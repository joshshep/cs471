#ifndef SUFFIX_TREE_NODE_H_
#define SUFFIX_TREE_NODE_H_

#include <map>
#include <vector>
//#include <assert.h>
#include <cassert>
#include <stdio.h>

#include <iostream>
#include <fstream>

namespace suffix_tree {

using std::cout;
using std::endl;

typedef struct sequence {
	std::string name;
	std::string bps;
} Sequence;

class SuffixTreeNode {
public:
	SuffixTreeNode(const char* str, int len, SuffixTreeNode* parent);
	
	~SuffixTreeNode();

	// exhausts the query along the suffix tree from this node. returns the child node of the edge on which this query
	// is exhausted
	// this is basically the same as findpath except we don't modify the suffix tree
	// in the initial call, match_len should equal 0
	const SuffixTreeNode* MatchStr(const char* query, int query_len, int &match_len) const;

	// we need find/create a path for the input string
	// returns: the newly created leaf node
	SuffixTreeNode* FindPath(const char* query, int query_len);

	// return v
	SuffixTreeNode* NodeHops(const char* beta, int beta_len);

	/*
	e.g.,
	given
	edge_label == aaaa
	index = 2
	this (aaaa)-> child

	we insert an intermediary internal node
	this (aa)-> newInternal (aa)-> child
	note: for this to remain a valid suffix tree, you MUST modify the returned node
	*/
	// index - the index at which to break
	// returns the new node
	SuffixTreeNode* BreakEdge(SuffixTreeNode* child, int index);

	// index - the index at which the edge and query differ
	SuffixTreeNode* InsertNode(SuffixTreeNode* child, const char* query, int query_len, int index);

	// sets the internal node ids starting with id i
	void SetInternalNodeIds(int & i);

	SuffixTreeNode* GetDeepestInternalNode();

	void GetDeepestInternalNode(SuffixTreeNode*& max_depth_node);

	void PrintParentage() const;

	void EnumerateDFS();
	
	// print the children of the current node dfs
	void PrintChildren() const;
	static void PrintNode(const SuffixTreeNode *n);

	bool IsLeaf() const;

	bool IsRoot() const;

	uint64_t TotalStrDepth() const;

	void TotalStrDepth(uint64_t& total) const;
	
	void PrintBWTindex();

	std::map<char, SuffixTreeNode*> children_;

	const char * incoming_edge_label_;
	int edge_len_;
	SuffixTreeNode* parent_; 

	int str_depth_;
	int id_ = -1;
	SuffixTreeNode* suffix_link_ = nullptr;

	int start_leaf_index_ = -1;
	int end_leaf_index_ = -1;

	//////////////////////////////////////////////////////////////////////
	// private
	//////////////////////////////////////////////////////////////////////    
private:
	void PrintChildrenShallow();

	// given a pointer to a specific node u in the tree, display u's children from left to right; 
	void PrintChildren(int node_depth);
	
	void Indent(int indentation);
};

} //namespace suffix_tree

#endif
