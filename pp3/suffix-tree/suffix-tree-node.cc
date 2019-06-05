#include "suffix-tree-node.h"

namespace suffix_tree {

SuffixTreeNode* SuffixTreeNode::MatchStr(const char* query, int query_len, int &match_len) {
	auto search = children_.find(query[0]);
	if (search == children_.end()){
		return this;
	}
	auto child = search->second;

	for (int i = 0; i < child->edge_len_; i++){
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
	auto search = children_.find(query[0]);
	if (search == children_.end()){
		// we need to add a child
		auto newChild = new SuffixTreeNode(query, query_len, this);
		return newChild;
	}
	auto child = search->second;

	for (int i = 0; i < child->edge_len_; i++){
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

	// we need to check this node's children
	// recurse
	return child->FindPath(query + child->edge_len_, query_len - child->edge_len_);
}

SuffixTreeNode* SuffixTreeNode::NodeHops(const char* beta, int beta_len) {
	if (beta_len==0) {
		return this;
	}
	auto search = children_.find(beta[0]);
	assert(search != children_.end());
	auto child = search->second;
	assert(child);

	if (beta_len < child->edge_len_) {
		// we need to break the edge and create a node
		return this->BreakEdge(child, beta_len);
	}

	// hop
	return child->NodeHops(beta + child->edge_len_, beta_len - child->edge_len_);
}

/*
this
  \
   \
    \
   child

this
  \
   \
  new_internal_node
        \
         \
        child
*/
SuffixTreeNode* SuffixTreeNode::BreakEdge(SuffixTreeNode* child, int index) {
	assert(index > 0);
	assert(index < child->edge_len_);
	
	auto new_internal_node = new SuffixTreeNode(child->incoming_edge_label_, index, this);
	new_internal_node->children_[child->incoming_edge_label_[index]] = child;

	
	child->parent_ = new_internal_node;
	child->incoming_edge_label_ += index;
	child->edge_len_ -= index;

	return new_internal_node;
}

/*
this
  \
   \
    \
   child

this
  \
   \
  new_internal_node
        \
         \
        child

this
  \
   \
  new_internal_node
    /       \
   /         \
child      newLeafNode
*/
SuffixTreeNode* SuffixTreeNode::InsertNode(SuffixTreeNode* child, const char* query, int query_len, int index) {
	auto new_internal_node = this->BreakEdge(child, index);
	auto new_leaf_node = new SuffixTreeNode(query + index, query_len - index, new_internal_node);

	return new_leaf_node;
}

} // namespace suffix_tree
