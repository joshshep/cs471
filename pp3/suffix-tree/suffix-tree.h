#ifndef SUFFIX_TREE_H_
#define SUFFIX_TREE_H_

#include <chrono>

#include "suffix-tree-node.h"


namespace suffix_tree {

class SuffixTree {
public:
	// we add 1 to length to count the null terminator
	SuffixTree(const char* str, int len);
	~SuffixTree();

	// builds the suffix tree with a simple series of FindPath calls
	void BuildTreeSimple();

	// builds the suffix tree using McCreight's algorithm
	// returns the number of nodes in the tree
	int BuildTreeMccreight();

	// SL(u) is known (root or internal node)
	SuffixTreeNode* Case1(SuffixTreeNode* prev_leaf, int index);

	// SL(u) is _not_ known (root or internal node)
	SuffixTreeNode* Case2(SuffixTreeNode* prev_leaf);

	void FindPath();

	void NodeHops();

	void PrintBuildStats();

	void PrintPath(SuffixTreeNode *descendant);
	
	void PrintPath(SuffixTreeNode *ancestor, SuffixTreeNode *descendant);

	void PrintTree() const;

	void GetLongestMatchingRepeat();

	void PrintBWTindex();

	void PrintBWTindex(SuffixTreeNode* n);


	SuffixTreeNode* root_ = nullptr;
	const char* str_;
	int len_;

};

} // namespace suffix_tree

#endif
