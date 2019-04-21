#include "read-map/read-map.h"

namespace read_map {

#define ZETA 25

ReadMap::ReadMap(Sequence & genome, std::vector<Sequence> reads) {
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

int ReadMap::Run() {
	// step 1: construct the suffix tree
	std::cout << "ReadMap: Step 1: construct the reference genome suffix tree" << std::endl;
	st_ = new suffix_tree::SuffixTree(genome_bps_, genome_len_);
	
	// step 2: prepare the suffix tree
	std::cout << "ReadMap: Step 2: build A" << std::endl;
	A_ = new int[genome_len_];
	next_index_ = 0;
	memset(A_, -1, sizeof(int) * genome_len_);
	PrepareST(st_->root_);

	// step 3:
	std::cout << "ReadMap: Step 3: ??" << std::endl;

	std::cout << "ReadMap: dealloc" << std::endl;
	delete A_;
	delete st_;
	return 0;
}

} // namespace read_map