#include "read-map/read-map.h"

namespace read_map {

// minimum exact match length
#define ZETA 25

ReadMap::ReadMap(Sequence & genome, std::vector<Sequence> & reads)
	: genome_(genome), reads_(reads) {
	genome_len_ = genome.bps.size();
	genome_bps_ = genome.bps.c_str();
}

int ReadMap::PrepareST(suffix_tree::SuffixTreeNode* node) {
	if (!node) {
		return 0;
	}
	if (node->IsLeaf()) {
		// is leaf
		A_[next_index_] = node->id_;
		if (node->str_depth_ >= ZETA) {
			node->start_leaf_index_ = next_index_;
			node->end_leaf_index_ = next_index_;
		}
		next_index_++;
		return 0;
	}

	// is node
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

	return 0;
}

suffix_tree::SuffixTreeNode* ReadMap::FindLoc(suffix_tree::Sequence & read) {
	suffix_tree::SuffixTreeNode* root = st_->root_;

	const char * bps = read.bps.c_str();
	int bps_len = read.bps.size();

	suffix_tree::SuffixTreeNode* deepest_node = nullptr;
	int deepest_node_depth = ZETA - 1;
	for (int i=0; i < bps_len - ZETA; i++) {
		int match_len = -1;
		// TODO use suffix links
		auto cand_deepest_node = root->MatchStr(bps + i, bps_len - i, match_len);
		assert(cand_deepest_node);

		// by the project assignment description, we must use this heuristic
		if (cand_deepest_node->str_depth_ > deepest_node_depth) {
			deepest_node_depth = cand_deepest_node->str_depth_;
			deepest_node = cand_deepest_node;
		}
	}
	assert(deepest_node);
	return deepest_node;
}

int ReadMap::Align(int genome_match_start, suffix_tree::Sequence & read) {
	const char* read_bps = read.bps.c_str();
	int read_len = read.bps.size();

	int genome_align_start = genome_match_start - read_len;
	genome_align_start = std::max(genome_align_start, 0);

	int genome_align_len = read_len * 2;
	genome_align_len = std::min(genome_align_len, genome_len_ - genome_align_start);

	// TODO: this is bad because it makes a copy
	std::string genome_substr = genome_.bps.substr(genome_align_start, genome_align_len);

	// from the assignment
	// {m_a =+1, m_i=-2, h=-5, g=-1}
	const aligner::ScoreConfig align_config = {1, -2, -5, -1};

	aligner::LocalAligner * local_aligner = new aligner::LocalAligner(genome_substr, read.bps, align_config);

	int alignment_score = local_aligner->Align(false);

	return alignment_score;
}

int ReadMap::CalcReadMapping(suffix_tree::Sequence & read) {
	auto deepest_node = FindLoc(read);
	for (int genome_match_start = deepest_node->start_leaf_index_; 
	     genome_match_start <= deepest_node->end_leaf_index_; 
	     genome_match_start++) {
		assert(genome_match_start >= 0);
		int alignment_score = Align(genome_match_start, read);
		
	}

	return 0;
}

int ReadMap::CalcReadMappings() {
	for (auto read : reads_) {
		CalcReadMapping(read);
	}
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