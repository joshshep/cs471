#ifndef READ_MAP_H_
#define READ_MAP_H_

#include <thread>
#include "read-map/read-map-worker.h"

namespace read_map {

class ReadMap {
public:
	ReadMap(Sequence & genome, std::vector<Sequence>& reads) : genome_(genome), reads_(reads) {
		
	}
	~ReadMap() {

	}

	// generates A
	void PrepareST(int *A, suffix_tree::SuffixTreeNode* node, int & next_index);

	std::vector<Strpos> LaunchThreads(const aligner::ScoreConfig& align_config, const suffix_tree::SuffixTree& st, const int * A, int num_threads=8) {
		cout << "Launching " << num_threads << " thread(s) ..." << endl;
		std::vector<ReadMapWorker *> workers(num_threads);
		std::vector<std::thread> threads;//(num_threads);
		int num_reads_rem = reads_.size();
		std::vector<Strpos> mappings(reads_.size());

		for (int tid=0; tid<num_threads; tid++) {
			int this_worker_num_reads = num_reads_rem / (num_threads - tid);
			workers[tid] = new ReadMapWorker(tid, genome_, reads_, align_config, st, A);

			threads.push_back(std::thread([worker = workers[tid], &mappings](int _istart_read, int _this_worker_num_reads) {
				cout << "_istart_read: " << _istart_read << " ; _this_worker_num_reads: " << _this_worker_num_reads << endl; 
				worker->CalcReadMappings(_istart_read, _this_worker_num_reads, mappings);
			}, reads_.size() - num_reads_rem, this_worker_num_reads));
			num_reads_rem -= this_worker_num_reads;
		}
		
		for (int tid=0; tid<num_threads; tid++) {
			threads[tid].join();
			delete workers[tid];
		}
		return mappings;
	}

	void SaveMappings(std::string ofname, std::vector<Strpos>& mappings) {
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


	void SaveMappingsStats(std::string ofname, std::vector<Strpos>& mappings) {
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

	void Run(std::string ofname) {
		// generate st
		cout << endl;
		cout << "Constructing the reference genome suffix tree..." << endl;
		const suffix_tree::SuffixTree * st = new suffix_tree::SuffixTree(genome_.bps.c_str(),  genome_.bps.size());

		// generate A
		cout << endl;
		cout << "Building A (PrepareST)..." << endl;
		int next_index = 0;
		int * A = new int[genome_.bps.size()];
		PrepareST(A, st->root_, next_index);
		
		// do the work
		cout << endl;
		cout << "Calculating the read mappings..." << endl;
		const aligner::ScoreConfig align_config = {1, -2, -5, -1};
		auto mappings = LaunchThreads(align_config, *st, A);

		cout << endl;
		cout << "Saving mappings stats..." << endl;
		SaveMappingsStats(ofname, mappings);

		cout << endl;
		cout << "Deallocating..." << endl;
		delete[] A;
		delete st;
	}

	Sequence & genome_;
	std::vector<Sequence> & reads_;
};

} // namespace read_map

#endif