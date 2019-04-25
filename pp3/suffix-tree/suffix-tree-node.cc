#include "suffix-tree-node.h"

namespace suffix_tree {

SuffixTreeNode* SuffixTreeNode::MatchStr(const char* query, int query_len, int &match_len) {
	auto search = children_.find(query[0]);
	if (search == children_.end()){
		//match_len = 0;
		return this;
	}
	auto child = search->second;

	for (int i=0; i<child->edge_len_; i++){
		if (i >= query_len){
			// the query string ends in the middle of an edge, so we need to insert a node
			match_len += i;
			return child;
		}
		if (query[i] != child->incoming_edge_label_[i]){
			// the edge labels do not match
			// we need to create a new node
			match_len += i;
			return child;
		}
	}

	// we need to check this node's children
	// recurse
	match_len += child->edge_len_;
	return child->MatchStr(query + child->edge_len_, query_len - child->edge_len_, match_len);
}

SuffixTreeNode* SuffixTreeNode::FindPath(const char* query, int query_len) {
	//printf("FindPath('%*.*s')\n", query_len, query_len, query);
	//assert(query_len > 0);

	auto search = children_.find(query[0]);
	if (search == children_.end()){
		// we need to add a child
		auto newChild = new SuffixTreeNode(query, query_len, this);
		children_[query[0]] = newChild;
		return newChild;
	}
	auto child = search->second;

	for (int i=0; i<child->edge_len_; i++){
		if (i >= query_len){
			// the query string ends in the middle of an edge, so we need to insert a node
			return this->InsertNode(child, query, query_len, i);
		}
		if (query[i] != child->incoming_edge_label_[i]){
			// the edge labels do not match
			// we need to create a new node
			return this->InsertNode(child, query, query_len, i);
		}
	}

	// we're at the end of the edge
	if (child->edge_len_ == query_len){
		// this suffix is already in the the tree
		// nothing to do
		// when does this happen?
		printf("error: suffix already exists in the tree?!\n");
		assert(0);
		exit(1);
		return nullptr;
	}

	// we need to check this node's children
	// recurse
	//printf("recurse FindPath(%*.*s)\n", query_len - child->edge_len_, query_len - child->edge_len_, query + child->edge_len_);
	return child->FindPath(query + child->edge_len_, query_len - child->edge_len_);
}

SuffixTreeNode* SuffixTreeNode::NodeHops(const char* beta, int beta_len) {
	//printf("NodeHops(%*.*s)\n", beta_len, beta_len, beta);
	if (beta_len==0) {
		//printf("beta_len == 0\n");
		return this;
	}
	auto search = children_.find(beta[0]);
	assert(search != children_.end());
	auto child = search->second;
	assert(child);

	if (beta_len < child->edge_len_) {
		// we need to break the edge and create a node
		//printf("NodeHops(): need to break an edge\n");
		return this->BreakEdge(child, beta_len);
	}

	// hop
	//printf("NodeHops(): hop to next node\n");
	return child->NodeHops(beta + child->edge_len_, beta_len - child->edge_len_);
}

SuffixTreeNode* SuffixTreeNode::BreakEdge(SuffixTreeNode* child, int index) {
	assert(index > 0);
	assert(index < child->edge_len_);
	
	auto newInternalNode = new SuffixTreeNode(child->incoming_edge_label_, index, this);
	newInternalNode->children_[child->incoming_edge_label_[index]] = child;

	
	child->parent_ = newInternalNode;
	child->incoming_edge_label_ += index;
	child->edge_len_ -= index;

	// this -> newInternalNode -> child
	return newInternalNode;
}

SuffixTreeNode* SuffixTreeNode::InsertNode(SuffixTreeNode* child, const char* query, int query_len, int index) {
	assert(index > 0);
	//edge_len_ -= index;
	//if (ed)
	//assert(edge_len_ > 0);

	auto newInternalNode = this->BreakEdge(child, index);
	auto newLeafNode = new SuffixTreeNode(query + index, query_len - index, newInternalNode);

	return newLeafNode;
}

} // namespace suffix_tree