#include "read-map/read-map.h"

#define ZETA 25

int PrepareST(SuffixTreeNode* node, int* A, int & next_index) {
	if (!node) {
		return 0;
	}
	if (node->IsLeaf()) {
		// is leaf
		A[next_index] = node->id_;
		if (node->str_depth_ >= ZETA) {
			node->start_leaf_index_ = next_index;
			node->end_leaf_index_ = next_index;
		}
		next_index++;
		return 0;
	}

	// is node
	for (auto child : node->children_) {
		PrepareST(child, A, next_index);
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

int ReadMap(Sequence & genome, std::vector<Sequence> reads) {
	const int genome_len = genome.bps.length;
	const char * genome_bps = genome.bps.c_str();

	// step 1: construct the suffix tree
	SuffixTree* st = new SuffixTree(genome_bps, genome_len);
	
	// step 2: prepare the suffix tree
	int* A = new int[genome_len];
	int next_index = 0;
	memset(A, -1, sizeof(int) * genome_len);
	PrepareST(st->root_, A, next_index);

	// step 3:


	delete A;
	delete st;
	return 0;
}