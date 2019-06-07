#ifndef SUFFIX_TREE_H_
#define SUFFIX_TREE_H_

#include <chrono>

#include "suffix-tree-node.h"


namespace suffix_tree {

class SuffixTree {
public:
	// we add 1 to length to count the null terminator
	SuffixTree(const char* str, int len) : str_(str), len_(len+1) {
		//BuildTreeSimple();
		//printf("input str length: %d\n", len_);
		//printf("Building suffix tree...\n");

		// simply build the tree
		BuildTreeMccreight();

		// build the tree with timing and stats
		//PrintBuildStats();
	}
	~SuffixTree(){
		delete root_;
	}

	// returns the number of nodes in the tree
	int BuildTreeMccreight();

	void PrintBuildStats() {
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

	void PrintPath(SuffixTreeNode *descendant) {
		if (descendant)
			PrintPath(root_, descendant);
	}
	
	void PrintPath(SuffixTreeNode *ancestor, SuffixTreeNode *descendant) {
		if (ancestor == descendant) {
			return;
		}
		PrintPath(ancestor, descendant->parent_);
		printf("%*.*s", descendant->edge_len_, descendant->edge_len_, descendant->incoming_edge_label_);
	}

	// SL(u) is known (root or internal node)
	SuffixTreeNode* Case1(SuffixTreeNode* prev_leaf, int index);

	// SL(u) is _not_ known (root or internal node)
	SuffixTreeNode* Case2(SuffixTreeNode* prev_leaf);

	void BuildTreeSimple(){
		root_ = new SuffixTreeNode(str_, len_, nullptr);
		for (int i=0; i<len_-1; i++){
			root_->FindPath(str_+i, len_-i);
		}
	}

	void PrintTree(){
		root_->PrintChildren();
	}

	void FindPath();

	void GetLongestMatchingRepeat() {
		SuffixTreeNode * n = root_->GetDeepestInternalNode();
		printf("string depth of deepest node: %d\n", n->str_depth_);
		printf("indices of longest exact matching repeat:");
		for (auto child : n->children_) {
			assert(child.second->IsLeaf());
			printf(" %d", child.second->id_);
		}
		printf("\n");
	}

	void NodeHops();

	SuffixTreeNode* root_ = nullptr;
	const char* str_;
	int len_;


	void PrintBWTindex(){
		PrintBWTindex(root_);
	}

	void PrintBWTindex(SuffixTreeNode* n){
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

};

} // namespace suffix_tree

#endif
