#include "suffix-tree/suffix-tree.h"

namespace suffix_tree {

SuffixTree::SuffixTree(const char* str, int len) : str_(str), len_(len+1) {
	//printf("input str length: %d\n", len_);
	//printf("Building suffix tree...\n");

	// simply build the tree
	BuildTreeMccreight();

	// build the tree with timing and stats
	// PrintBuildStats();
}

SuffixTree::~SuffixTree(){
	delete root_;
}

void SuffixTree::BuildTreeSimple(){
	root_ = new SuffixTreeNode(str_, len_, nullptr);
	for (int i=0; i<len_-1; i++){
		root_->FindPath(str_+i, len_-i);
	}
}

int SuffixTree::BuildTreeMccreight() {
	// the incoming edge label to the root doesn't matter
	assert(root_ == nullptr);
	root_ = new SuffixTreeNode("root", 4, nullptr);

	// manually set root to have its suffix link point to itself
	root_->suffix_link_ = root_;
	root_->parent_ = root_;

	auto prev_leaf = root_;

	// u is the parent of the previously created leaf
	// u' is the parent of u 
	// v is the parent of the new leaf to be created
	// v' is the parent of v
	for (int i = 0; i < len_; i++) {
		auto u = prev_leaf->parent_;
		if (u->suffix_link_) {
			// SL(u) is known
			// Case1A and Case1B are the same
			prev_leaf = Case1(prev_leaf, i);
		} else {
			// SL(u) is not known yet
			// Case2A and Case2B are the almost the same
			prev_leaf = Case2(prev_leaf);
		}
		prev_leaf->id_ = i;
	}

	// yes we set the internal node id's after everything
	// its because these id's don't matter
	int num_nodes = len_;

	// SetInternalNodeIds modifies num_nodes inplace
	root_->SetInternalNodeIds(num_nodes);
	return num_nodes;
}

SuffixTreeNode* SuffixTree::Case1(SuffixTreeNode* prev_leaf, int index) {
	auto u = prev_leaf->parent_;
	auto v = u->suffix_link_;
	int alpha = v->str_depth_;

	//printf("case1() passed str: %*.*s\n", len_ - index - alpha, len_ - index - alpha, str_ + index + alpha);

	return v->FindPath(str_ + index + alpha, len_ - index - alpha);
}

SuffixTreeNode* SuffixTree::Case2(SuffixTreeNode* prev_leaf) {
	auto u = prev_leaf->parent_;
	auto u_prime = u->parent_;
	auto v_prime = u_prime->suffix_link_;
	// when u_prime == v_prime == root, alpha_prime is 0
	auto beta = u->incoming_edge_label_;
	auto beta_len = u->edge_len_;
	if (u_prime->IsRoot()) {
		// this is the difference between case A and case B
		beta++; beta_len--;
	}
	auto v = v_prime->NodeHops(beta, beta_len);
	u->suffix_link_ = v;

	auto new_leaf = v->FindPath(prev_leaf->incoming_edge_label_, prev_leaf->edge_len_);
	return new_leaf;
}

void SuffixTree::PrintBuildStats() {
	// time build
	auto t1 = std::chrono::high_resolution_clock::now();
	int num_nodes = BuildTreeMccreight();
	auto t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();

	// print stats
	cout << "Time elapsed while building suffix tree: " << duration << " microsecond(s)" << endl;
	cout << "total number of nodes in the tree: " << num_nodes << endl;
	cout << "number of internal nodes in the tree: " << num_nodes - len_ << endl;
	auto total_str_depth = root_->TotalStrDepth();
	double avg_depth = (double) total_str_depth / (num_nodes - len_);
	cout << "avg str depth of internal node: " << avg_depth << endl;
}

void SuffixTree::PrintPath(SuffixTreeNode *descendant) {
	if (descendant)
		PrintPath(root_, descendant);
}

void SuffixTree::PrintPath(SuffixTreeNode *ancestor, SuffixTreeNode *descendant) {
	if (ancestor == descendant) {
		return;
	}
	PrintPath(ancestor, descendant->parent_);
	printf("%*.*s", descendant->edge_len_, descendant->edge_len_, descendant->incoming_edge_label_);
}

void SuffixTree::PrintTree() const {
	root_->PrintChildren();
}

void SuffixTree::GetLongestMatchingRepeat() {
	SuffixTreeNode * n = root_->GetDeepestInternalNode();
	printf("string depth of deepest node: %d\n", n->str_depth_);
	printf("indices of longest exact matching repeat:");
	for (auto child : n->children_) {
		assert(child.second->IsLeaf());
		printf(" %d", child.second->id_);
	}
	printf("\n");
}

void SuffixTree::PrintBWTindex(){
	PrintBWTindex(root_);
}

void SuffixTree::PrintBWTindex(SuffixTreeNode* n){
	if (n->children_.size() == 0) {
		// Why not just zero index? ugh ...
		if (n->id_==0) {
			printf("%c\n", str_[len_-1]);
		} else {
			printf("%c\n", str_[n->id_-1]);
		}
		return;
	}
	for (auto child : n->children_){
		PrintBWTindex(child.second);
	}
}

} // namespace suffix_tree
