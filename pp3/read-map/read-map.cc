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

} // namespace read_map