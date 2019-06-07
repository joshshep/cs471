#include "suffix-tree-node.h"

namespace suffix_tree {

SuffixTreeNode::SuffixTreeNode(const char* str, int len, SuffixTreeNode* parent)
	: incoming_edge_label_(str), edge_len_(len), parent_(parent) {
	if (parent) {
		parent->children_[incoming_edge_label_[0]] = this;
		str_depth_ = parent->str_depth_ + edge_len_;
	} else {
		str_depth_ = 0;
	}
}

SuffixTreeNode::~SuffixTreeNode() {
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

const SuffixTreeNode* SuffixTreeNode::MatchStr(const char* query, int query_len, int &match_len) const {
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
	this->children_[child->incoming_edge_label_[0]] = new_internal_node;
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
child      new_leaf_node
*/
SuffixTreeNode* SuffixTreeNode::InsertNode(SuffixTreeNode* child, const char* query, int query_len, int index) {
	auto new_internal_node = this->BreakEdge(child, index);
	auto new_leaf_node = new SuffixTreeNode(query + index, query_len - index, new_internal_node);

	return new_leaf_node;
}

// sets the internal node ids starting with id i
void SuffixTreeNode::SetInternalNodeIds(int & i){
	if (id_ == -1) {
		id_ = i;
		i++;
	}
	for (auto child : children_) {
		child.second->SetInternalNodeIds(i);
	}
}

SuffixTreeNode* SuffixTreeNode::GetDeepestInternalNode() {
	SuffixTreeNode* deepest_internal_node = this;
	GetDeepestInternalNode(deepest_internal_node);
	return deepest_internal_node;
}

void SuffixTreeNode::GetDeepestInternalNode(SuffixTreeNode*& max_depth_node) {
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

void SuffixTreeNode::PrintParentage() const {
	printf("[%d] is the parent of [%d]\n", this->parent_->id_, this->id_);
	for (auto child : children_) {
		child.second->PrintParentage();
	}
}

// print the children of the current node dfs
void SuffixTreeNode::PrintChildren() const {
	PrintNode(this);
	printf(" (root)\n");
	for (auto child : children_){
		// starting at node depth 1
		child.second->PrintChildren(1);
	}
}
void SuffixTreeNode::PrintNode(const SuffixTreeNode *n) {
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

bool SuffixTreeNode::IsLeaf() const {
	return this->children_.size() == 0;
}

bool SuffixTreeNode::IsRoot() const {
	return this->suffix_link_ == this;
}
uint64_t SuffixTreeNode::TotalStrDepth() const {
	uint64_t total = 0;
	TotalStrDepth(total);
	return total;
}
void SuffixTreeNode::TotalStrDepth(uint64_t& total) const {
	if (children_.size() != 0) {
		total += str_depth_;
	}
	for (auto child : children_){
		child.second->TotalStrDepth(total);
	}
}

//  ----------------------------------------------
// private
void SuffixTreeNode::PrintChildrenShallow(){
	for (auto child : children_){
		printf(" ");
		PrintNode(child.second);
	}
	printf("\n");
}

// given a pointer to a specific node u in the tree, display u's children from left to right; 
void SuffixTreeNode::PrintChildren(int node_depth){
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
void SuffixTreeNode::Indent(int indentation){
	// printf is really cool
	printf("%*s", indentation*2, "");
}

} // namespace suffix_tree
