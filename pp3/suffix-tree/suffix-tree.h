#ifndef SUFFIX_TREE_H_
#define SUFFIX_TREE_H_

#include <chrono>

#include "suffix-tree-node.h"


namespace suffix_tree {


class SuffixTree {
public:
    SuffixTree(const char* str, int len) : str_(str), len_(len) {
        //BuildTreeSimple();
        printf("input str length: %d\n", len);
        printf("Building suffix tree...\n");

        // simply build the tree
        BuildTreeMccreight();

        // build the tree with timing and stats
        //PrintBuildStats();
    }
    ~SuffixTree(){
        delete root_;
    }

    /*
    SuffixTreeNode* MatchStr(const char* query, int query_len, int & match_len) {
        return root_->MatchStr()
    }
    */

    // returns the number of nodes in the tree
    int BuildTreeMccreight();

    void PrintBuildStats() {
        // time build
        auto t1 = std::chrono::high_resolution_clock::now();
        int num_nodes = BuildTreeMccreight();
        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();

        // print stats
        printf("Time elapsed while building suffix tree: %llu microsecond(s)\n", duration);
        printf("total number of nodes in the tree: %d\n", num_nodes);
        printf("number of internal nodes in the tree: %d\n", num_nodes - len_);
        auto total_str_depth = root_->TotalStrDepth();
        double avg_depth = (double) total_str_depth / (num_nodes - len_);
        printf("avg str depth of internal node: %lf\n", avg_depth);
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

} // namespace suffix_tree

#endif