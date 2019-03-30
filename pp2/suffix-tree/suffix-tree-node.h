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
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist6(0,999); // distribution in range [1, 6]

        id_ = dist6(rng);
        //id_ = num_nodes_;
        //num_nodes_++;
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

    // we need find/create a path for the input string
    // returns: the newly created leaf node
    SuffixTreeNode* FindPath(const char* query, int query_len) {
        printf("FindPath('%*.*s')\n", query_len, query_len, query);
        assert(query_len > 0);

        auto search = children_.find(query[0]);
        if (search == children_.end()){
            // we need to add a child
            auto newChild = new SuffixTreeNode(query, query_len, this);
            children_[query[0]] = newChild;
            return newChild;
        }
        auto child = search->second;
        assert(child);

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
            printf("error: suffix already exists in tree?!\n");
            printf("parent: ");
            SuffixTreeNode::PrintNode(this);
            printf("\n");
            printf("child: ");
            SuffixTreeNode::PrintNode(child);
            printf("\n");
            printf("query str: %*.*s\n", query_len, query_len, query);
            assert(0);
            exit(1);
            return nullptr;
        }

        // we need to check this node's children
        // recurse
        printf("recurse FindPath(%*.*s)\n", query_len - child->edge_len_, query_len - child->edge_len_, query + child->edge_len_);

        return child->FindPath(query + child->edge_len_, query_len - child->edge_len_);
    }

    // return v
    SuffixTreeNode* NodeHops(const char* beta, int beta_len) {
        printf("NodeHops(%*.*s)\n", beta_len, beta_len, beta);
        if (beta_len==0) {
            printf("beta_len == 0\n");
            return this;
        }
        auto search = children_.find(beta[0]);
        assert(search != children_.end());
        auto child = search->second;
        assert(child);

        if (beta_len > child->edge_len_){
            // hop
            printf("NodeHops(): hop to next node\n");
            return child->NodeHops(beta + child->edge_len_, beta_len - child->edge_len_);
        }
        
        if (beta_len == child->edge_len_) {
            // the node exists
            printf("NodeHops(): the node simply already exists\n");
            return child;
        }


        //FindPath(beta, beta_len);

        // we need to break the edge and create a node
        // TODO findpath or insertnode?!

        printf("NodeHops(): need to break an edge\n");
        return this->BreakEdge(child, beta_len);
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
    SuffixTreeNode* BreakEdge(SuffixTreeNode* child, int index) {
        assert(index > 0);
        assert(index < child->edge_len_);
        
        auto newInternalNode = new SuffixTreeNode(child->incoming_edge_label_, index, this);
        newInternalNode->children_[child->incoming_edge_label_[index]] = child;

        this->children_[newInternalNode->incoming_edge_label_[0]] = newInternalNode;

        child->incoming_edge_label_ += index;
        child->edge_len_ -= index;

        // this -> newInternalNode -> child
        return newInternalNode;
    }

    // index - the index at which the edge and query differ
    SuffixTreeNode* InsertNode(SuffixTreeNode* child, const char* query, int query_len, int index) {
        assert(index > 0);
        //edge_len_ -= index;
        //if (ed)
        //assert(edge_len_ > 0);

        auto newInternalNode = this->BreakEdge(child, index);
        auto newLeafNode = new SuffixTreeNode(query + index, query_len - index, newInternalNode);

        newInternalNode->children_[newLeafNode->incoming_edge_label_[0]] = newLeafNode;

        return newLeafNode;
    }

    void EnumerateDFS();

    
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

    bool IsRoot() {
        return this->suffix_link_ == this;
    }
    

    void PrintBWTindex();

    std::map<char, SuffixTreeNode*> children_;

    const char * incoming_edge_label_; //?
    int edge_len_; //?
    SuffixTreeNode* parent_; 

    int str_depth_;
	int id_;
    SuffixTreeNode* suffix_link_ = nullptr;

    const char kStrTerminator = '$';

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

#endif