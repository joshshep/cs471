#ifndef SUFFIX_TREE_H_
#define SUFFIX_TREE_H_

#include "suffix-tree-node.h"

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
        // the incoming edge label doesn't matter
        root_ = new SuffixTreeNode("root", 4, nullptr);
        // manually set root to have its suffix link point to itself
        root_->suffix_link_ = root_;
        root_->parent_ = root_;

        auto prev_leaf = root_;

        for (int i=0; i<len_-1; i++) {
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
                // Case2A and Case2B are the almost the same
                printf("case2\n");
                prev_leaf = Case2(prev_leaf);
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
            // this is the difference between case A and case B
            beta++; beta_len--;
        }
        auto v = v_prime->NodeHops(beta, beta_len);
        assert(v);


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