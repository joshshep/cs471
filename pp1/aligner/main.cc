#include "aligner/global-aligner.h"

int main(int argc, char *argv[]) {
	// defaults 
	// TODO where should I put this?
	const int kMinNumArgs = 3;
	const int kMaxNumArgs = 4;
	const char * kDfltConfigFname = "params.config";

	if (argc < kMinNumArgs) {
		std::cout << "Error: too few arguments" << std::endl;
		print_help();
		exit(1);
	} else if (argc > kMaxNumArgs) {
		std::cout << "Error: too many arguments" << std::endl;
		print_help();
		exit(1);
	}

	const char * fasta_fname = argv[1];
	auto sequences = load_sequences(fasta_fname);
	
	const char * alignment_str = argv[2];
	AlignmentScope align_scope = parse_align_scope(alignment_str);

	const char * config_fname = kDfltConfigFname;
	if (argc == kMinNumArgs + 1) {
		// config specified
		config_fname = argv[kMinNumArgs];
	}
	ScoreConfig scores = load_config(config_fname);

	if (align_scope == GLOBAL) {
		GlobalAligner aligner(sequences.first.bps, sequences.second.bps, scores);
		int align_score = aligner.Align();
		std::cout << "Alignment score: " << align_score << std::endl;
	} else {
		std::cout << "Error: local alignment has not been implemented" << std::endl;
		exit(1);
	}
	return 0;
}