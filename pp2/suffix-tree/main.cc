#include "suffix-tree/suffix-tree.h"

int main(int argc, char *argv[]) {
	// defaults 
	// TODO where should I put these constants?
	const int kNumArgs = 3;

	if (argc < kNumArgs) {
		std::cout << "Error: too few arguments" << std::endl;
		//PrintHelp();
		exit(1);
	} else if (argc > kNumArgs) {
		std::cout << "Error: too many arguments" << std::endl;
		//PrintHelp();
		exit(1);
	}

	// load sequences from fasta based on cmd arg
	const char * fasta_fname = argv[1];
	auto sequence = LoadSequence(fasta_fname);

	sequence.bps += '$';
	const char * seq = sequence.bps.c_str();
	int seq_len = sequence.bps.size();
	SuffixTree* st = new SuffixTree(seq, seq_len);
	st->PrintTree();

	/*
	SuffixTreeNode * stn = new SuffixTreeNode("foobar", 69, nullptr);
	stn->FindPath("aaaa$", 5);
	stn->FindPath("aabb$", 5);
	stn->PrintChildren();
	*/
	
	return 0;
}