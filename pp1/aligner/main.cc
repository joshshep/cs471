#include "aligner/global-aligner.h"
#include "aligner/local-aligner.h"

int main(int argc, char *argv[]) {
	// defaults 
	// TODO where should I put this?
	const int kMinNumArgs = 3;
	const int kMaxNumArgs = 4;
	const char * kDfltConfigFname = "params.config";

	if (argc < kMinNumArgs) {
		std::cout << "Error: too few arguments" << std::endl;
		PrintHelp();
		exit(1);
	} else if (argc > kMaxNumArgs) {
		std::cout << "Error: too many arguments" << std::endl;
		PrintHelp();
		exit(1);
	}

	const char * fasta_fname = argv[1];
	auto sequences = LoadSequences(fasta_fname);
	
	const char * alignment_str = argv[2];
	AlignmentScope align_scope = ParseAlignScope(alignment_str);

	const char * config_fname = kDfltConfigFname;
	if (argc == kMinNumArgs + 1) {
		// config specified
		config_fname = argv[kMinNumArgs];
	}
	ScoreConfig scores = LoadConfig(config_fname);

	if (align_scope == GLOBAL) {
		GlobalAligner aligner(sequences.first.bps, sequences.second.bps, sequences, scores);
		aligner.Align();
	} else {
		LocalAligner aligner(sequences.first.bps, sequences.second.bps, sequences, scores);
		aligner.Align();
	}
	return 0;
}