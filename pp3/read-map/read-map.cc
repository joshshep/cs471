#include "read-map/read-map.h"

namespace read_map {

void ReadMap::PrepareST(int *A, suffix_tree::SuffixTreeNode* node, int & next_index) {
	if (!node) {
		return;
	}
	if (node->IsLeaf()) {
		// is leaf
		A[next_index] = node->id_;
		if (node->str_depth_ >= ZETA) {
			node->start_leaf_index_ = next_index;
			node->end_leaf_index_ = next_index;
		}
		next_index++;
		return;
	}

	// is internal node (so it must have children)
	for (auto child : node->children_) {
		PrepareST(A, child.second, next_index);
	}

	// now we set the internal node's start/end index
	if (node->str_depth_ >= ZETA) {
		auto first_child = node->children_.begin()->second;
		auto last_child = node->children_.rbegin()->second;
		node->start_leaf_index_ = first_child->start_leaf_index_;
		node->end_leaf_index_ = last_child->end_leaf_index_;
	}

	return;
}

std::vector<Strpos> ReadMap::LaunchThreads(const suffix_tree::SuffixTree& st, const int * A, int num_threads) {
	cout << "Launching " << num_threads << " thread(s)..." << endl;
	std::vector<ReadMapWorker *> workers(num_threads);
	std::vector<std::thread> threads;
	int num_reads_rem = reads_.size();
	std::vector<Strpos> mappings(reads_.size());

	for (int tid=0; tid<num_threads; tid++) {
		// calculate the number of reads that this worker needs to map
		int this_worker_num_reads = num_reads_rem / (num_threads - tid);
		workers[tid] = new ReadMapWorker(tid, genome_, reads_, align_config_, st, A);

		// launch the thread on a lambda that calculates the mappings
		threads.push_back(std::thread([worker = workers[tid], &mappings](int _istart_read, int _this_worker_num_reads) {
			printf("_istart_read: %d ; _this_worker_num_reads: %d\n", _istart_read, _this_worker_num_reads);
			worker->CalcReadMappings(_istart_read, _this_worker_num_reads, mappings);
		}, reads_.size() - num_reads_rem, this_worker_num_reads));

		// update the remaining number of reads that need to be mapped
		num_reads_rem -= this_worker_num_reads;
	}
	
	// wait for the threads to terminate
	for (int tid=0; tid<num_threads; tid++) {
		threads[tid].join();
		delete workers[tid];
	}
	return mappings;
}

void ReadMap::SaveMappings(std::string ofname, std::vector<Strpos>& mappings) {
	assert(mappings.size() == reads_.size());
	std::ofstream ofile(ofname);

	// keep track of the number of reads that were not mapped (either due to MIN_PROP_IDENTITY or MIN_PROP_LENGTH_COVERAGE)
	int n_mapless = 0;

	// write the csv header
	ofile << "Read name,Start index of hit,End index of hit" << endl;
	for (int i = 0; i < (int)mappings.size(); i++) {
		auto & mapping = mappings[i];
		auto & read = reads_[i];
		if (mapping.start < 0) {
			ofile << read.name << ",no mapping found" << endl;
			n_mapless++;
		} else {
			ofile << read.name << "," << mapping.start << "," << mapping.start + mapping.len << endl;
		}
	}
	int n_reads = (int)reads_.size();
	double perc_attempted = (double)(100 * (n_reads - n_mapless)) / n_reads;
	printf("%d / %d = %lf%% attempted mappings\n", n_reads - n_mapless, n_reads, perc_attempted);
}

void ReadMap::SaveMappingsStats(std::string ofname, std::vector<Strpos>& mappings) {
	assert(mappings.size() == reads_.size());
	std::ofstream ofile(ofname);

	// keep track of the number of reads that were not mapped (either due to MIN_PROP_IDENTITY or MIN_PROP_LENGTH_COVERAGE)
	int n_mapless = 0;

	// keep track of the number of reads that were mapped to a location in the genome, but at an incorrect position
	// the correct position is parsed from the read Sequence's name field
	int incorrect_mappings = 0;
	const int mapping_idx_threshold = 5; // characters

	// write the csv header
	ofile << "Read name,Start index of hit,End index of hit" << endl;
	for (int i = 0; i < (int)mappings.size(); i++) {
		auto & mapping = mappings[i];
		auto & read = reads_[i];
		if (mapping.start < 0) {
			ofile << read.name << ",no mapping found" << endl;
			n_mapless++;
		} else {
			ofile << read.name << "," << mapping.start << "," << mapping.start + mapping.len << endl;
			// TODO we could error check in this parsing process i.e., breakout
			int str_idx = read.name.rfind('_') + 1;
			int read_idx = std::stoi(read.name.substr(str_idx));
			int dislocation = std::abs(read_idx - mapping.start);
			if (dislocation > mapping_idx_threshold) {
				incorrect_mappings++;
			}
		}
	}
	int n_reads = (int)reads_.size();
	int correct_mappings = n_reads - n_mapless - incorrect_mappings;
	double perc_attempted = (double)(100 * (n_reads - n_mapless)) / n_reads;
	double perc_correct = (double)(100 * correct_mappings) / n_reads;
	printf("%d / %d = %lf%% attempted mappings\n", n_reads - n_mapless, n_reads, perc_attempted);
	printf("%d / %d = %lf%% correct mappings (within %d chars)\n", correct_mappings, n_reads, perc_correct, mapping_idx_threshold);
}

void ReadMap::Run(std::string ofname) {
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// generate st
	cout << endl;
	cout << "Constructing the reference genome suffix tree..." << endl;
	auto t1 = high_resolution_clock::now();

	const suffix_tree::SuffixTree * st = new suffix_tree::SuffixTree(genome_.bps.c_str(),  genome_.bps.size());

	auto t2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>( t2 - t1 ).count();
	cout << "Time elapsed: " << duration << " microseconds" << endl;

	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// generate A
	cout << endl;
	cout << "Building A (PrepareST)..." << endl;
	t1 = high_resolution_clock::now();

	int next_index = 0;
	int * A = new int[genome_.bps.size()];
	PrepareST(A, st->root_, next_index);

	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	cout << "Time elapsed: " << duration << " microseconds" << endl;
	
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// do the work
	cout << endl;
	cout << "Calculating the read mappings..." << endl;
	t1 = high_resolution_clock::now();

	auto mappings = LaunchThreads(*st, A, NUM_MAPPING_THREADS);

	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	cout << "Time elapsed: " << duration << " microseconds" << endl;

	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// save to file
	cout << endl;
	cout << "Saving mappings stats..." << endl;
	t1 = high_resolution_clock::now();
	SaveMappingsStats(ofname, mappings);

	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	cout << "Time elapsed: " << duration << " microseconds" << endl;

	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// deallocate
	cout << endl;
	cout << "Deallocating..." << endl;
	t1 = high_resolution_clock::now();

	delete[] A;
	delete st;

	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	cout << "Time elapsed: " << duration << " microseconds" << endl;
}

} // namespace read_map