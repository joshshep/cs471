#include "read-map/read-map.h"

namespace read_map {

ReadMap::ReadMap(Sequence & genome, std::vector<Sequence> & reads)
	: genome_(genome), reads_(reads) {
	genome_len_ = genome.bps.size();
	genome_bps_ = genome.bps.c_str();

	// from the assignment
	// {m_a =+1, m_i=-2, h=-5, g=-1}
	const aligner::ScoreConfig align_config = {1, -2, -5, -1};
	local_aligner_= new aligner::LocalAligner(MAX_READ_LEN * 2, MAX_READ_LEN, align_config);
}

ReadMap::~ReadMap() {
	delete local_aligner_;
}

int ReadMap::PrepareST(suffix_tree::SuffixTreeNode* node) {
	if (!node) {
		return 0;
	}
	if (node->IsLeaf()) {
		// is leaf
		A_[next_index_] = node->id_;
		// TODO DEBUG TEST: remove this true shortcut eval
		if (node->str_depth_ >= ZETA) {
			node->start_leaf_index_ = next_index_;
			node->end_leaf_index_ = next_index_;
		}
		next_index_++;
		return 0;
	}

	// is internal node (so it must have children)
	for (auto child : node->children_) {
		PrepareST(child.second);
	}

	// now we set the internal node's start/end index
	// TODO DEBUG TEST: remove this true shortcut eval
	if (node->str_depth_ >= ZETA) {
		auto first_child = node->children_.begin()->second;
		auto last_child = node->children_.rbegin()->second;
		node->start_leaf_index_ = first_child->start_leaf_index_;
		node->end_leaf_index_ = last_child->end_leaf_index_;
	}

	return 0;
}

suffix_tree::SuffixTreeNode* ReadMap::FindLoc(std::string & read) {
	suffix_tree::SuffixTreeNode* root = st_->root_;

	const char * read_bps = read.c_str();
	int read_len = read.size();

	suffix_tree::SuffixTreeNode* longest_match_node = nullptr;
	suffix_tree::SuffixTreeNode* cur_node = root; // i.e., T
	int longest_match_len = ZETA - 1;
	assert(ZETA > 0);
	for (int i = 0; i < read_len - ZETA + 1; i++) {
		int match_len = cur_node->str_depth_;

		// i.e., u
		auto cand_longest_match_node = cur_node->MatchStr(
			read_bps + i + cur_node->str_depth_, 
			read_len - i - cur_node->str_depth_, 
			match_len
		);
		assert(cand_longest_match_node);
		/*
		printf("- i=%d : read='%s'\n", i, read_bps + i);
		printf("  candidate deepest str depth: %d\n", cand_deepest_node->str_depth_);
		printf("  match_len = %d\n", match_len);
		printf("  num children = %d\n", (int)cand_deepest_node->children_.size());
		*/

		// by the project assignment description, we must use this heuristic
		//printf("  [%d] match_len = %d\n", i, match_len);
		if (match_len > longest_match_len) {
			longest_match_len = match_len;
			longest_match_node = cand_longest_match_node;
		}
		if (!cand_longest_match_node->suffix_link_) {
			// we are at a leaf node
			// use our parents suffix link
			// note: this will work as long as the root has children
			cur_node = cand_longest_match_node->parent_->suffix_link_;
			assert(cur_node);
		} else {
			cur_node = cand_longest_match_node->suffix_link_;
			assert(cur_node);
		}
	}
	return longest_match_node;
}

int ReadMap::Align(int genome_align_start, std::string & read, aligner::AlignmentStats & alignment_stats) {
	int read_len = read.size();

	int genome_align_len = read_len * 2;
	genome_align_len = std::min(genome_align_len, genome_len_ - genome_align_start);

	local_aligner_->SetOperands(genome_bps_ + genome_align_start, genome_align_len, read.c_str(), read_len);

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
		std::cout << "Warning: failed to find " << ZETA << " character exact match for read named '" << read.name << "'" << std::endl;
		return {-1, -1};
	}
	//std::cout << "Found deepest node for '" << read.name << "'" << std::endl;
	//std::cout << "deepest_node str_depth: " << deepest_node->str_depth_ << std::endl;
	//std::cout << "read: " << read.bps << std::endl;
	int longest_align_len = -1;
	int read_len = read.bps.size();
	int read_map_loc = -1;
	for (int leaf_index = deepest_node->start_leaf_index_; 
	     leaf_index <= deepest_node->end_leaf_index_; 
	     leaf_index++) {
		assert(leaf_index >= 0);
		int genome_match_start = A_[leaf_index];
		aligner::AlignmentStats alignment_stats;

		int genome_align_start = genome_match_start - read_len;
		genome_align_start = std::max(genome_align_start, 0);

		// perform the alignment
		int alignment_score = Align(genome_align_start, read.bps, alignment_stats);

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
		/*
		printf("  %% identity: %lf\n", prop_identity);
		printf("  len coverage: %lf\n", len_coverage);
		printf("  start index: %d + %d = %d\n", genome_align_start, alignment_stats.startIndex, genome_align_start + alignment_stats.startIndex);
		printf("  %*.*s\n", align_len, align_len, genome_bps_ + genome_align_start + alignment_stats.startIndex);


		printf("  [%d] alignment_score: %d\n", genome_match_start, alignment_score);
		*/
	}

	if (read_map_loc < 0) {
		std::cout << "Failed to find a suitable alignment for '" << read.name << "'" << std::endl;
	}

	//printf("  [%d] match\n", read_map_loc);

	return {read_map_loc, longest_align_len};
}

std::vector<Strpos> ReadMap::CalcReadMappings() {
	std::vector<Strpos> mappings(reads_.size());
	for (int i = 0; i < (int)reads_.size(); i++) {
		auto & read = reads_[i];
		if (i % 5000 == 0) {
			printf("%d/%d reads mapped\n", i, (int)reads_.size());
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
	for (int i = 0; i < (int)mappings.size(); i++) {
		auto & mapping = mappings[i];
		auto & read = reads_[i];
		if (mapping.start < 0) {
			ofile << read.name << ",no match found" << std::endl;
		} else {
			ofile << read.name << "," << mapping.start << "," << mapping.start + mapping.len << std::endl;
		}
	}
}

int ReadMap::Run() {
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::microseconds;
	// step 1: construct the suffix tree
	std::cout << std::endl;
	std::cout << "***** ReadMap: Step 1 *****" << std::endl;
	std::cout << "Constructing the reference genome suffix tree..." << std::endl;
	auto t1 = high_resolution_clock::now();
	st_ = new suffix_tree::SuffixTree(genome_bps_, genome_len_);
    auto t2 = high_resolution_clock::now();
	auto duration = duration_cast<microseconds>( t2 - t1 ).count();
	std::cout << "Time elapsed: " << duration << " microseconds" << std::endl;
	
	// step 2: prepare the suffix tree
	std::cout << std::endl;
	std::cout << "***** ReadMap: Step 2 *****" << std::endl;
	std::cout << "Building A (PrepareST)..." << std::endl;
	t1 = high_resolution_clock::now();
	A_ = new int[genome_len_];
	next_index_ = 0;
	memset(A_, -1, sizeof(int) * genome_len_);
	PrepareST(st_->root_);
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	std::cout << "Time elapsed: " << duration << " microseconds" << std::endl;

	// step 3: map the reads
	std::cout << std::endl;
	std::cout << "***** ReadMap: Step 3 *****" << std::endl;
	std::cout << "Calculating the read mappings..." << std::endl;
	t1 = high_resolution_clock::now();
	auto mappings = CalcReadMappings();
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	std::cout << "# of alignments / read: " << (double)n_aligns_/reads_.size() << std::endl;
	std::cout << "Time elapsed: " << duration << " microseconds" << std::endl;

	// step 4: write to the output file
	std::cout << std::endl;
	std::cout << "***** ReadMap: Step 4 *****" << std::endl;
	std::string ofname = "output.csv";
	std::cout << "Writing output to '" << ofname << "'..." << std::endl;
	t1 = high_resolution_clock::now();
	SaveMappings(ofname, mappings);
	t2 = high_resolution_clock::now();
	duration = duration_cast<microseconds>( t2 - t1 ).count();
	std::cout << "Time elapsed: " << duration << " microseconds" << std::endl;

	std::cout << "ReadMap: dealloc" << std::endl;
	delete A_;
	delete st_;
	return 0;
}

} // namespace read_map