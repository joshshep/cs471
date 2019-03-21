#include "suffix-tree-builder/suffix-tree-builder.h"

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
	
	return 0;
}