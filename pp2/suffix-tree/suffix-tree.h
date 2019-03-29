#ifndef SUFFIX_TREE_H_
#define SUFFIX_TREE_H_

#include <map>
#include <vector>
//#include <assert.h>
#include <cassert>
#include <stdio.h>

#include <iostream>
#include <fstream>
#include <vector>

typedef struct sequence {
	std::string name;
	std::string bps;
} Sequence;

class SuffixTreeNode {
public:
    SuffixTreeNode(const char* str, int len, SuffixTreeNode* parent)
    : incoming_edge_label_(str), edge_len_(len), parent_(parent){
        if (parent) {
            parent->children_[str[0]] = this;
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

    // TODO what does this return value mean?
    // we need find/create a path for the input string
    bool FindPath(const char* query, int query_len) {
        assert(query_len > 0);

        auto search = children_.find(query[0]);
        if (search == children_.end()){
            // we need to add a child
            children_[query[0]] = new SuffixTreeNode(query, query_len, this);
            return false;
        }
        auto child = search->second;
        assert(child);

        for (int i=0; i<child->edge_len_; i++){
            if (i >= query_len){
                // the query string ends in the middle of an edge, so we need to insert a node
                InsertNode(child, query, query_len, i);
                return true;
            }
            if (query[i] != child->incoming_edge_label_[i]){
                // the edge labels do not match
                // we need to create a new node
                InsertNode(child, query, query_len, i);
                return false;
            }
        }

        // we're at the end of the edge
        if (child->edge_len_ == query_len){
            // this suffix is already in the the tree
            // nothing to do
            // when does this happen?
            printf("error: suffix already exists in tree?!\n");
            assert(0);
            return true;
        }

        // we need to check this node's children
        // recurse
        return child->FindPath(query + child->edge_len_, query_len - child->edge_len_);
    }

    SuffixTreeNode* NodeHops(char* beta, int beta_len) {
        assert(beta_len > 0);
        auto search = children_.find(beta[0]);
        assert(search != children_.end());
        auto child = search->second;
        assert(child);

        if (beta_len > edge_len_){
            // hop
            return child->NodeHops(beta+edge_len_, beta_len-edge_len_);
        }
        if (beta_len == edge_len_) {
            // the node exists
            return child;
        }

        //FindPath(beta, beta_len);

        // we need to break the edge and create a node

        // TODO change findpath return type
        return nullptr;
    }
    /*
    aaaa
    @ - a - a - a - a - $ - @

    aabb
    @ - a - a - @ - a - a - $ - @
                 \ 
                  - b - b - $ - @
    */

   /*
   // at the current node, insert the
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
    // insert a node between "this" and the parameter child
    SuffixTreeNode* InsertNode(char* newStr, int len, int index) {
        assert(newStr[0] == incoming_edge_label_[0]);
        auto existingChild = children_[newStr[0]];
        existingChild->incoming_edge_label_ += index;
        auto newNode = new SuffixTreeNode(newStr, index, this);
        children_[newStr[0]] = newNode;
        return newNode;
    }
    */

    // index - the index at which the edge and query differ
    SuffixTreeNode* InsertNode(SuffixTreeNode* child, const char* query, int query_len, int index) {
        edge_len_ -= index;
        assert(edge_len_ > 0);

        child->incoming_edge_label_ += index;
        child->edge_len_ -= index;
        assert(child->edge_len_ > 0);


        auto newInternalNode = new SuffixTreeNode(query, index, this);
        newInternalNode->children_[child->incoming_edge_label_[0]] = child;

        // the leaf adds itself to its parent's children_ list
        assert(query_len - index > 0);
        
        //auto _newLeafNode = new SuffixTreeNode(query+index, query_len - index, newInternalNode);
        new SuffixTreeNode(query+index, query_len - index, newInternalNode);

        return newInternalNode;
    }

    void EnumerateDFS();

    void Indent(int indentation){
        // printf is really cool
        printf("%*s", indentation*2, "");
    }
    void PrintChildren(){
        printf("root children:");
        PrintChildrenShallow();
        for (auto child : children_){
            child.second->PrintChildren(1);
        }
    }
    void PrintChildrenShallow(){
        for (auto child : children_){
            printf(" ");
            PrintNode(child.second);
        }
        printf("\n");
    }
    static void PrintNode(SuffixTreeNode *n){
        printf("(%*.*s)", n->edge_len_, n->edge_len_, n->incoming_edge_label_);
    }
    // given a pointer to a specific node u in the tree, display u's children from left to right; 
    void PrintChildren(int ichild){
        Indent(ichild);
        PrintNode(this);
        if (children_.size() == 0){
            printf(" LEAF!\n");
            return;
        }
        printf(" children:");
        PrintChildrenShallow();
        for (auto child : children_){
            child.second->PrintChildren(ichild+1);
        }
    }

    void PrintBWTindex();

    std::map<char, SuffixTreeNode*> children_;

    const char * incoming_edge_label_; //?
    int edge_len_; //?
    SuffixTreeNode* parent_; 

    int str_depth_;
	int id_;
    SuffixTreeNode* suffix_link_;

    const char kStrTerminator = '$';
};  

class SuffixTree {
public:
    SuffixTree(const char* str, int len) : str_(str), len_(len) {
        BuildTreeSimple();
    }

    void BuildTreeSimple(){
        root_ = new SuffixTreeNode(str_, len_, nullptr);
        for (int i=0; i<len_-1; i++){
            root_->FindPath(str_+i, len_-i);
        }
    }

    void PrintTree(){
        root_->PrintChildren();
    }

    void McCormick();

    void FindPath();

    void NodeHops();

    SuffixTreeNode* root_;
    const char* str_;
    int len_;

};

////////////////////////////////
// printing 
////////////////////////////////

// prints the help to run this executable 
void PrintHelp();

// pretty-prints the size value as quantity of {B, KiB, MiB, GiB}
void PrintSize(size_t asize);


///////////////////////////////////////
// loading from files
///////////////////////////////////////

// loads exactly 1 sequence from the provided file name
Sequence LoadSequence(const char* fasta_fname);


///////////////////////////////////////////
// misc free helpers (TODO)
///////////////////////////////////////////

// formats the input string (representing base pairs) to lowercase
void Format_bps(std::string & str);

// trims the provided whitespace characters from the provided string
std::string Trim(const std::string& str, const std::string& whitespace = " ");

// determines the length of the input integer
char intLen(int n);

#endif