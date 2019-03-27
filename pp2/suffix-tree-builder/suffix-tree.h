#ifndef SUFFIX_TREE_H_
#define SUFFIX_TREE_H_

#include <map>
#include <vector>
#include <assert.h>
#include <stdio.h>

class SuffixTreeNode {
public:
    SuffixTreeNode(const char* str, int len, SuffixTreeNode* parent)
    : data_(str), data_len_(len), parent_(parent){
        parent->children_[str[0]] = this;
    }
    
    ~SuffixTreeNode(){
        for (auto child : children_){
            // if the edge ends in '$', then the node is null
            if (child.second != nullptr){
                delete child.second;
            }
        }
    }

    // TODO what does this return value mean?
    bool FindPath(char* query, int query_len) {
        assert(query_len > 0);

        int i;
        for (i=0; i<edge_len_; i++){
            if (i >= query_len){
                // we are in the middle of an edge, so we need to insert a node
                // TODO
                return true;
            }
            if (query[i] != incoming_edge_label_[i]){
                // the edge labels do not match
                // we need to create a new node
                return false;
            }
        }

        // we're at the end of the edge
        assert(i == edge_len_);
        if (i == query_len){
            // this suffix is already in the the tree
            // nothing to do
            return true;
        }

        auto search = children_.find(query[0]);
        if (search == children_.end()){
            // we need to add a child
            children_[query[i]] = new SuffixTreeNode(query+i, query_len - i, this);
            return false;
        }
        auto child = search->second;
        // TODO nullptr check (i.e., '$')
        return child->FindPath(query+i, query_len-i);
    }

    SuffixTreeNode* NodeHops(char* beta, int beta_len) {
        assert(beta_len > 0);
        auto search = children_.find(beta[0]);
        assert(search != children_.end());
        auto child = search->second;
        assert(child);

        if (beta_len > edge_len_){
            return child->NodeHops(beta+edge_len_, beta_len-edge_len_);
        }
        if (beta_len == edge_len_) {
            return child;
        }

        FindPath(beta, beta_len);

        // we need to break the edge and create a node
        // TODO change findpath return type
        return nullptr;
    }
    /*
    @ - a - c - t - g - $ - @

    @ - a - c - @ - t - g - $ - @
                 \ 
                  $ - @
    */

    SuffixTreeNode* InsertNode(char* newStr, int len) {
        for(int i=0; i<len && i<edge_len_; i++) {
            if (newStr[i] != incoming_edge_label_[i]) {
                return InsertNode(newStr, len, i);
            }
        }
        // this shouldn't happen
        printf("Failed to insert node\n");
        assert(0);
        return nullptr;
    }

    // TODO figure out who calls this (parent or child)
    SuffixTreeNode* InsertNode(char* newStr, int len, int index) {
        assert(newStr[0] == incoming_edge_label_[0]);
        auto existingChild = children_[newStr[0]];
        existingChild->incoming_edge_label_ += index;
        auto newNode = new SuffixTreeNode(newStr, index, this);
        children_[newStr[0]] = newNode;
        return newNode;
    }

    void EnumerateDFS();

    // given a pointer to a specific node u in the tree, display u's children from left to right; 
    void PrintChildren();

    void PrintBWTindex();

    SuffixTreeNode* parent_; 
    std::map<char, SuffixTreeNode*> children_;

    const char * data_;
    int data_len_;

    char * incoming_edge_label_; //?
    int edge_len_; //?

    int str_depth_;
	int id_;
    SuffixTreeNode* suffix_link_;

    const char kStrTerminator = '$';
};  

class SuffixTree {
public:
    SuffixTree(char* str, int len) {
        root_ = new SuffixTreeNode(str, len, nullptr);

    }

    void FindPath();

    void NodeHops();

    SuffixTreeNode* root_;

};

#endif