#include "read-map/read-map.h"

namespace read_map {

ReadMap::ReadMap(Sequence & genome, std::vector<Sequence> & reads)
	: genome_(genome), reads_(reads) {
	genome_len_ = genome.bps.size();
	genome_bps_ = genome.bps.c_str();

	// from the assignment
	// {m_a =+1, m_i=-2, h=-5, g=-1}
	const aligner::ScoreConfig align_config = {1, -2, -5, -1};
	local_aligner_= new aligner::LocalAligner(MAX_READ_LEN, MAX_READ_LEN, align_config);
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
		if (true || node->str_depth_ >= ZETA) {
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
	if (true || node->str_depth_ >= ZETA) {
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

	suffix_tree::SuffixTreeNode* deepest_node = nullptr;
	int deepest_node_depth = ZETA - 1;
	assert(ZETA > 0);
	for (int i=0; i < read_len - ZETA + 1; i++) {
		//printf("i=%d ; bps_len - ZETA = %d\n", i, bps_len - ZETA);
		int match_len = -1;
		// TODO use suffix links
		auto cand_deepest_node = root->MatchStr(read_bps + i, read_len - i, match_len);
		assert(cand_deepest_node);
		/*
		printf("- i=%d : read='%s'\n", i, read_bps + i);
		printf("  candidate deepest str depth: %d\n", cand_deepest_node->str_depth_);
		printf("  match_len = %d\n", match_len);
		printf("  num children = %d\n", cand_deepest_node->children_.size());
		*/
		// by the project assignment description, we must use this heuristic
		if (cand_deepest_node->str_depth_ + match_len > deepest_node_depth) {
			deepest_node_depth = cand_deepest_node->str_depth_ + match_len;
			deepest_node = cand_deepest_node;
		}
	}
	return deepest_node;
}

int ReadMap::Align(int genome_match_start, std::string & read) {
	int read_len = read.size();

	int genome_align_start = genome_match_start - read_len;
	genome_align_start = std::max(genome_align_start, 0);

	int genome_align_len = read_len * 2;
	genome_align_len = std::min(genome_align_len, genome_len_ - genome_align_start);

	// TODO: this is bad because it makes a copy
	std::string genome_substr = genome_.bps.substr(genome_align_start, genome_align_len);

	int alignment_score = local_aligner_->Align(false);

	return alignment_score;
}

int ReadMap::CalcReadMapping(suffix_tree::Sequence & read) {
	auto deepest_node = FindLoc(read.bps);
	if (deepest_node == nullptr) {
		std::cout << "Warning: failed to find " << ZETA << " character exact match for read named '" << read.name << "'" << std::endl;
		return -1;
	}
	std::cout << "Found deepest node for '" << read.name << "'" << std::endl;
	std::cout << "  deepest_node str_depth: " << deepest_node->str_depth_ << std::endl;
	for (int leaf_index = deepest_node->start_leaf_index_; 
	     leaf_index <= deepest_node->end_leaf_index_; 
	     leaf_index++) {
		assert(leaf_index >= 0);
		int genome_match_start = A_[leaf_index];
		int alignment_score = Align(genome_match_start, read.bps);
		printf("  [%d] alignment_score: %d\n", genome_match_start, alignment_score);
	}

	return 0;
}

int ReadMap::CalcReadMappings() {
	int i = 0;
	for (auto read : reads_) {
		if (i % 5000 == 0) {
			printf("%d/%d reads complete\n", i, (int)reads_.size());
		}
		CalcReadMapping(read);
		i++;
	}
	printf("%d/%d reads complete\n", i, (int)reads_.size());

	return 0;
}

int ReadMap::Run() {
	// step 1: construct the suffix tree
	std::cout << "***** ReadMap: Step 1 *****" << std::endl;
	std::cout << "Constructing the reference genome suffix tree..." << std::endl;
	st_ = new suffix_tree::SuffixTree(genome_bps_, genome_len_);
	
	// step 2: prepare the suffix tree
	std::cout << "***** ReadMap: Step 2 *****" << std::endl;
	std::cout << "Building A (PrepareST)..." << std::endl;
	A_ = new int[genome_len_];
	next_index_ = 0;
	memset(A_, -1, sizeof(int) * genome_len_);
	PrepareST(st_->root_);

	// step 3:
	std::cout << "***** ReadMap: Step 3 *****" << std::endl;
	std::cout << "Calculating the read mappings..." << std::endl;
	CalcReadMappings();

	std::cout << "ReadMap: dealloc" << std::endl;
	delete A_;
	delete st_;
	return 0;
}

} // namespace read_map