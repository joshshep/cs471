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

    // TODO what does this return value mean?
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
        printf("root children:");
        PrintChildrenShallow();
        for (auto child : children_){
            child.second->PrintChildren(1);
        }
    }
    static void PrintNode(SuffixTreeNode *n){
        if (n->suffix_link_) {
            auto sf = n->suffix_link_;
            printf("([%d] %*.*s SF->[%d])", 
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
    void Indent(int indentation){
        // printf is really cool
        printf("%*s", indentation*2, "");
    }
};  

class SuffixTree {
public:
    SuffixTree(const char* str, int len) : str_(str), len_(len) {
        //BuildTreeSimple();
        BuildTreeMccreight();
    }

    /*static bool IsRoot(SuffixTreeNode* n){
        return n->parent_ == n;
    }*/

    void BuildTreeMccreight(){
        root_ = new SuffixTreeNode(str_, len_, nullptr);
        // manually set root to have its suffix link point to itself
        root_->suffix_link_ = root_;

        auto prev_leaf = root_->FindPath(str_, len_);

        for (int i=1; i<len_-1; i++) {
            auto u = prev_leaf->parent_;
            printf("\n***********************\n");
            printf("suffix: %s\n", str_+i);
            printf("u==");
            SuffixTreeNode::PrintNode(u);
            printf("\n");
            if (u->suffix_link_) {
                // SL(u) is known
                // Case1A and Case1B are the same
                printf("case1\n");
                prev_leaf = Case1(prev_leaf, i);
            } else {
                // SL(u) is not known yet
                // Case2A and Case2B are the same?!
                printf("case2\n");
                prev_leaf = Case2(prev_leaf);
                //printf("exiting prematurely to test case2...\n");
                //return;
                /*
                SuffixTreeNode* u_prime = u->parent_;
                if (IsRoot(u_prime)) {
                    // u' is the root
                    Case2B(prev_leaf, i);
                } else {
                    // u' is _not_ the root
                    Case2A(prev_leaf, i);
                }
                */
            }
            PrintTree();

        }
        

        // u is the parent of the previously created leaf
        // u' is the parent of u 
        // v is the parent of the new leaf to be created
        // v' is the parent of v

    }

    // SL(u) is known
    SuffixTreeNode* Case1(SuffixTreeNode* prev_leaf, int index) {
        auto u = prev_leaf->parent_;
        auto v = u->suffix_link_;
        int alpha = v->str_depth_;

        printf("case1() passed str: %*.*s\n", len_ - index - alpha, len_ - index - alpha, str_ + index + alpha);

        return v->FindPath(str_ + index + alpha, len_ - index - alpha);
    }

    // SL(u) is _not_ known
    SuffixTreeNode* Case2(SuffixTreeNode* prev_leaf) {
        auto u = prev_leaf->parent_;
        auto u_prime = u->parent_;
        assert(u_prime);
        auto v_prime = u_prime->suffix_link_;
        if (v_prime == nullptr) {
            printf("ERROR: v_prime is null in case2\n");
            PrintTree();
        }
        assert(v_prime);
        // when u_prime == v_prime == root, alpha_prime is 0
        auto beta = u->incoming_edge_label_;
        auto beta_len = u->edge_len_;
        if (u_prime->IsRoot()) {
            beta++; beta_len--;
        }
        auto v = v_prime->NodeHops(beta, beta_len);
        assert(v);

        //auto newLeaf = v_something->FindPath(str_ + index + v_something->str_depth_, len_ - index - v_something->str_depth_);
        //auto v = newLeaf->parent_;

        printf("setting suffix link: ");
        SuffixTreeNode::PrintNode(u);
        printf("  points to  ");
        SuffixTreeNode::PrintNode(v);
        printf("\n");

        u->suffix_link_ = v;

        auto new_leaf = v->FindPath(prev_leaf->incoming_edge_label_, prev_leaf->edge_len_);
        return new_leaf;
    }
    /*
    // SL(u) is unknown and u' is _not_ the root
    SuffixTreeNode* Case2A(SuffixTreeNode* prev_leaf, int index) {
        SuffixTreeNode* u = prev_leaf->parent_;
        SuffixTreeNode* u_prime = u->parent_;
    }

    // SL(u) is unknown and u' is the root
    SuffixTreeNode* Case2B(SuffixTreeNode* prev_leaf, int index) {
        SuffixTreeNode* u = prev_leaf->parent_;
        SuffixTreeNode* u_prime = u->parent_;

        
    }*/

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