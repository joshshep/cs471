#include "read-map/read-map.h"

#define ZETA 25

ReadMap::ReadMap(Sequence & genome, std::vector<Sequence> reads) {
	genome_len_ = genome.bps.length;
	genome_bps_ = genome.bps.c_str();
}

int ReadMap::PrepareST(SuffixTreeNode* node) {
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
	st_ = new SuffixTree(genome_bps_, genome_len_);
	
	// step 2: prepare the suffix tree
	A_ = new int[genome_len_];
	next_index_ = 0;
	memset(A_, -1, sizeof(int) * genome_len_);
	PrepareST(st_->root_);

	// step 3:


	delete A_;
	delete st_;
	return 0;
}