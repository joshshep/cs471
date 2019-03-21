#include "suffix-tree-builder/suffix-tree-builder.h"

int main(int argc, char *argv[]) {
	// defaults 
	// TODO where should I put these constants?
	const int kMinNumArgs = 3;
	const int kMaxNumArgs = 4;

	if (argc < kMinNumArgs) {
		std::cout << "Error: too few arguments" << std::endl;
		//PrintHelp();
		exit(1);
	} else if (argc > kMaxNumArgs) {
		std::cout << "Error: too many arguments" << std::endl;
		//PrintHelp();
		exit(1);
	}

	// load sequences from fasta based on cmd arg
	const char * fasta_fname = argv[1];
	//auto sequences = LoadSequences(fasta_fname);
	
	return 0;
}