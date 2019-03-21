#ifndef SUFFIX_TREE_BUILDER_H_
#define SUFFIX_TREE_BUILDER_H_

#include <iostream>

typedef struct sequence {
	std::string name;
	std::string bps;
} Sequence;

class SuffixTreeBuilder {

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

// loads exactly 2 sequences from the provided file name
std::pair<Sequence, Sequence> LoadSequences(const char* fasta_fname);


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