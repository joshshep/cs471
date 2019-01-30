#include "src/aligner.h"

int main(int argc, char *argv[]) {
	if (argc < MIN_N_ARGS) {
		std::cout << "Error: too few arguments" << std::endl;
		print_help();
		exit(1);
	} else if (argc > MAX_N_ARGS) {
		std::cout << "Error: too many arguments" << std::endl;
		print_help();
		exit(1);
	}

	const char * fasta_fname = argv[1];
	std::vector<std::string> sequences = load_sequences(fasta_fname);
	
	const char * alignment_str = argv[2];
	ALIGN_SCOPE align_scope = parse_align_scope(alignment_str);

	const char * config_fname = DFLT_CONFIG_FNAME;
	if (argc == MIN_N_ARGS + 1) {
		// config specified
		config_fname = argv[MIN_N_ARGS];
	}
	SCORE_CONFIG scores = load_config(config_fname);

	if (align_scope == GLOBAL) {
		int align_score = align_global(sequences[0], sequences[1], scores);
        std::cout << "Alignment score: " << align_score << std::endl;
	} else {
		std::cout << "Error: local alignment has not been implemented" << std::endl;
		exit(1);
	}
	return 0;
}