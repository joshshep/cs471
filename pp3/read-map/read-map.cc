#include "read-map/read-map.h"

namespace read_map {

ReadMap::ReadMap(Sequence & genome, std::vector<Sequence> & reads, const aligner::ScoreConfig& align_config)
	: genome_(genome), reads_(reads) {
	genome_len_ = genome.bps.size();
	genome_bps_ = genome.bps.c_str();

	local_aligner_= new aligner::LocalAligner(MAX_READ_LEN * 2, MAX_READ_LEN, align_config);
}

ReadMap::~ReadMap() {
	delete local_aligner_;
}

void ReadMap::PrepareST(suffix_tree::SuffixTreeNode* node) {
	if (!node) {
		return;
	}
	if (node->IsLeaf()) {
		// is leaf
		A_[next_index_] = node->id_;
		if (node->str_depth_ >= ZETA) {
			node->start_leaf_index_ = next_index_;
			node->end_leaf_index_ = next_index_;
		}
		next_index_++;
		return;
	}

	// is internal node (so it must have children)
	for (auto child : node->children_) {
		PrepareST(child.second);
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

suffix_tree::SuffixTreeNode* ReadMap::FindLoc(std::string & read) {
	const char * read_bps = read.c_str();
	int read_len = read.size();

	suffix_tree::SuffixTreeNode* longest_match_node = nullptr;
	suffix_tree::SuffixTreeNode* search_src = st_->root_; // i.e., T
	int longest_match_len = ZETA - 1;
	assert(ZETA > 0);
	for (int i = 0; i < read_len - search_src->str_depth_ - ZETA + 1; i++) {
		int match_len = search_src->str_depth_;

		const char * query = read_bps + i + search_src->str_depth_;
		int query_len = read_len - i - search_src->str_depth_;
		assert(query_len > 0);
		auto cand_longest_match_node = search_src->MatchStr(query, query_len, match_len);
		assert(cand_longest_match_node);

		// check if this is the best node encountered thus far
		if (match_len > longest_match_len) {
			longest_match_len = match_len;
			longest_match_node = cand_longest_match_node;
		}

		// jump with our suffix link
		if (match_len == cand_longest_match_node->str_depth_ && cand_longest_match_node->suffix_link_) {
			// use this node's suffix link
			search_src = cand_longest_match_node->suffix_link_;
		} else {
			// use this node's parent's suffix link
			search_src = cand_longest_match_node->parent_->suffix_link_;
		}
		assert(search_src);
	}
	return longest_match_node;
}

int ReadMap::Align(int genome_align_start, std::string & read, aligner::AlignmentStats & alignment_stats) {
	const char * read_bps = read.c_str();
	int read_len = read.size();

	int genome_align_len = read_len * 2;
	genome_align_len = std::min(genome_align_len, genome_len_ - genome_align_start);

	local_aligner_->SetOperands(genome_bps_ + genome_align_start, genome_align_len, read_bps, read_len);
	
	// perform the alignment
	int alignment_score = local_aligner_->Align(false);

	// gather stats about the alignment
	local_aligner_->CountRetraceStats(alignment_stats);

	n_aligns_++; // keep track of the number of times we have aligned reads
	return alignment_score;
}

Strpos ReadMap::CalcReadMapping(suffix_tree::Sequence & read) {
	auto deepest_node = FindLoc(read.bps);
	if (deepest_node == nullptr) {
		//cout << "Warning: failed to find " << ZETA << " character exact match for read named '" << read.name << "'" << endl;
		return {-1, -1};
	}

	// iterate through each child node of this deepest_node
	int longest_align_len = -1;
	int read_len = read.bps.size();
	int read_map_loc = -1;
	for (int leaf_index = deepest_node->start_leaf_index_; 
	     leaf_index <= deepest_node->end_leaf_index_; 
	     leaf_index++) {
		assert(leaf_index >= 0);
		int genome_match_start = A_[leaf_index];
		aligner::AlignmentStats alignment_stats;

		// we have the exact match location
		// now we need to offset this in order to perform the alignment
		int genome_align_start = genome_match_start - read_len;
		genome_align_start = std::max(genome_align_start, 0);

		// perform the alignment
		Align(genome_align_start, read.bps, alignment_stats);

		// handle the result
		int align_len = alignment_stats.nMatches + alignment_stats.nMismatches + alignment_stats.nGaps;
		double prop_identity = (double)alignment_stats.nMatches / align_len;
		double len_coverage = (double)align_len / read_len;

		if (prop_identity > MIN_PROP_IDENTITY && len_coverage > MIN_PROP_LENGTH_COVERAGE) {
			// good match
			if (align_len > longest_align_len) {
				// new best match
				longest_align_len = align_len;
				read_map_loc = genome_align_start + alignment_stats.startIndex;
			}
		}
	}

	if (read_map_loc < 0) {
		//cout << "Failed to find a suitable alignment for '" << read.name << "'" << endl;
	}
	return {read_map_loc, longest_align_len};
}

std::vector<Strpos> ReadMap::CalcReadMappings() {
	std::vector<Strpos> mappings(reads_.size());
	for (int i = 0; i < (int)reads_.size(); i++) {
		auto & read = reads_[i];
		if (i % 5000 == 0) {
			cout << i << "/" << reads_.size() << " reads mapped" << endl;
		}
		auto astrpos = CalcReadMapping(read);
		mappings[i] = astrpos;
	}
	printf("%d/%d reads complete\n", (int)reads_.size(), (int)reads_.size());

	return mappings;
}

void ReadMap::SaveMappings(std::string ofname, std::vector<Strpos>& mappings) {
	assert(mappings.size() == reads_.size());
	std::ofstream ofile(ofname);
	// keep track of the number of reads that we're not mapped (either due to MIN_PROP_IDENTITY or MIN_PROP_LENGTH_COVERAGE)
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
	int n_mapless = 0;
	int incorrect_mappings = 0;
	const int mapping_idx_threshold = 5; // characters

	// header
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

int ReadMap::Run(std::string ofname) {
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::microseconds;

	// step 1: construct the suffix tree
	cout << endl;
	cout << "***** ReadMap: Step 1 *****" << endl;
	cout << "Constructing the reference genome suffix tree..." << endl;
	auto t1 = high_resolution_clock::now();
	st_ = new suffix_tree::SuffixTree(genome_bps_, genome_len_);
	auto t2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>( t2 - t1 ).count();
	cout << "Time elapsed: " << duration << " microseconds" << endl;
	
	// step 2: prepare the suffix tree
	cout << endl;
	cout << "***** ReadMap: Step 2 *****" << endl;
	cout << "Building A (PrepareST)..." << endl;
	t1 = high_resolution_clock::now();
	A_ = new int[genome_len_];
	next_index_ = 0;
	memset(A_, -1, sizeof(int) * genome_len_);
	PrepareST(st_->root_);
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	cout << "Time elapsed: " << duration << " microseconds" << endl;

	// step 3: map the reads
	cout << endl;
	cout << "***** ReadMap: Step 3 *****" << endl;
	cout << "Calculating the read mappings..." << endl;
	t1 = high_resolution_clock::now();
	auto mappings = CalcReadMappings();
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	cout << "# of alignments / read: " << (double)n_aligns_/reads_.size() << endl;
	cout << "Time elapsed: " << duration << " microseconds" << endl;

	// step 4: write to the output file
	cout << endl;
	cout << "***** ReadMap: Step 4 *****" << endl;
	cout << "Writing output to '" << ofname << "'..." << endl;
	t1 = high_resolution_clock::now();
	SaveMappings(ofname, mappings);
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	cout << "Time elapsed: " << duration << " microseconds" << endl;

	cout << "ReadMap: deallocating..." << endl;
	delete A_;
	delete st_;
	return 0;
}

} // namespace read_map