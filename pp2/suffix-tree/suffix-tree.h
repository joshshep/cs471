#ifndef SUFFIX_TREE_H_
#define SUFFIX_TREE_H_

#include "suffix-tree-node.h"

class SuffixTree {
public:
    SuffixTree(const char* str, int len) : str_(str), len_(len) {
        //BuildTreeSimple();
        BuildTreeMccreight();
    }

    void BuildTreeMccreight();

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

    void NodeHops();

    SuffixTreeNode* root_;
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