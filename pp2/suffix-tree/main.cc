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
	// since we assume the fasta file is valid, we can ignore the alphabet file

	// load sequences from fasta based on cmd arg
	const char * fasta_fname = argv[1];
	auto sequence = LoadSequence(fasta_fname);


	sequence.bps += '$';
	const char * seq = sequence.bps.c_str();
	int seq_len = sequence.bps.size();
	SuffixTree* st = new SuffixTree(seq, seq_len);

	// print dfs
	//st->PrintTree();

	// TOGGLE
	st->PrintBWTindex();
	st->GetLongestMatchingRepeat();

	delete st;
	
	return 0;
}