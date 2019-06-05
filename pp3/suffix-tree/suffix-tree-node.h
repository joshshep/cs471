#ifndef SUFFIX_TREE_NODE_H_
#define SUFFIX_TREE_NODE_H_

#include <map>
#include <vector>
//#include <assert.h>
#include <cassert>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iostream>

namespace suffix_tree {

using std::cout;
using std::endl;

typedef struct sequence {
	std::string name;
	std::string bps;
} Sequence;

class SuffixTreeNode {
public:
	SuffixTreeNode(const char* str, int len, SuffixTreeNode* parent)
	: incoming_edge_label_(str), edge_len_(len), parent_(parent){
		if (parent) {
			parent->children_[incoming_edge_label_[0]] = this;
			str_depth_ = parent->str_depth_ + edge_len_;
		} else {
			str_depth_ = 0;
		}
	}
	
	~SuffixTreeNode(){
		for (auto child : children_){
			// if the edge ends in '$', then the node is null
			if (child.second != nullptr){
				delete child.second;
			} else {
				printf("error: null child?!\n");
				assert(0);
			}
		}
	}

	// exhausts the query along the suffix tree from this node. returns the child node of the edge on which this query
	// is exhausted
	// this is basically the same as findpath except we don't modify the suffix tree
	// in the initial call, match_len should equal 0
	SuffixTreeNode* MatchStr(const char* query, int query_len, int &match_len);

	// we need find/create a path for the input string
	// returns: the newly created leaf node
	SuffixTreeNode* FindPath(const char* query, int query_len);

	// return v
	SuffixTreeNode* NodeHops(const char* beta, int beta_len);

	SuffixTreeNode* GetDeepestInternalNode() {
		SuffixTreeNode* deepest_internal_node = this;
		GetDeepestInternalNode(deepest_internal_node);
		return deepest_internal_node;
	}

	// sets the internal node ids starting with id i
	void SetInternalNodeIds(int & i){
		if (id_ == -1) {
			id_ = i;
			i++;
		}
		for (auto child : children_) {
			child.second->SetInternalNodeIds(i);
		}
	}

	void GetDeepestInternalNode(SuffixTreeNode*& max_depth_node) {
		for (auto child : children_) {
			if (child.second->IsLeaf()) {
				continue;
			}
			child.second->GetDeepestInternalNode(max_depth_node);
		}
		if (this->str_depth_ > max_depth_node->str_depth_) {
			max_depth_node = this;
		}
	}

	void PrintParentage() {
		printf("[%d] is the parent of [%d]\n", this->parent_->id_, this->id_);
		for (auto child : children_) {
			child.second->PrintParentage();
		}
	}

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

	void EnumerateDFS();
	
	// print the children of the current node dfs
	void PrintChildren(){
		PrintNode(this);
		printf(" (root)\n");
		for (auto child : children_){
			// starting at node depth 1
			child.second->PrintChildren(1);
		}
	}
	static void PrintNode(SuffixTreeNode *n){
		if (n->suffix_link_) {
			auto sf = n->suffix_link_;
			printf("([%d] %*.*s SL->[%d])", 
				n->id_, 
				n->edge_len_,  n->edge_len_,  n->incoming_edge_label_, 
				sf->id_);
		} else {
			printf("([%d] %*.*s)", n->id_, 
				n->edge_len_, n->edge_len_, n->incoming_edge_label_);
		}
	}

	bool IsLeaf() {
		return this->children_.size() == 0;
	}

	bool IsRoot() {
		return this->suffix_link_ == this;
	}
	uint64_t TotalStrDepth() {
		uint64_t total = 0;
		TotalStrDepth(total);
		return total;
	}
	void TotalStrDepth(uint64_t& total){
		if (children_.size() != 0) {
			total += str_depth_;
		}
		for (auto child : children_){
			child.second->TotalStrDepth(total);
		}
	}
	

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
	void PrintChildrenShallow(){
		for (auto child : children_){
			printf(" ");
			PrintNode(child.second);
		}
		printf("\n");
	}

	// given a pointer to a specific node u in the tree, display u's children from left to right; 
	void PrintChildren(int node_depth){
		Indent(node_depth);
		PrintNode(this);
		if (children_.size() == 0){
			printf("\n");
			return;
		}
		printf("\n");
		for (auto child : children_){
			child.second->PrintChildren(node_depth+1);
		}
	}
	void Indent(int indentation){
		// printf is really cool
		printf("%*s", indentation*2, "");
	}
};

} //namespace suffix_tree

#endif
