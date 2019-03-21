#ifndef SUFFIX_TREE_H_
#define SUFFIX_TREE_H_

#include <vector> 

class SuffixTreeNode {
public:

    SuffixTreeNode();

    ~SuffixTreeNode() {
        for (auto child : children_){
            delete child;
        }
    }

    void EnumerateDFS();

    // given a pointer to a specific node u in the tree, display u's children from left to right; 
    void PrintChildren();

    void PrintBWTindex();

    SuffixTreeNode& parent_; 
    std::vector<SuffixTreeNode &> children_;
    char parent_edge_label_; //?
    int str_depth;
	int id_;
    SuffixTreeNode& suffix_link_;
};

class SuffixTree {
public:

    SuffixTree(char* input_str, int len);

    void FindPath();

    void NodeHops();

    SuffixTreeNode & root;

};

#endif