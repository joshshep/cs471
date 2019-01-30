#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define MIN_N_ARGS 3
#define MAX_N_ARGS 4
#define DFLT_CONFIG_FNAME "parameters.config"

enum ALIGN_SCOPE {GLOBAL, LOCAL};

typedef struct score_config {
	int match;
	int mismatch;
	int h; // start gap penalty
	int g; // continuing gap penalty
} SCORE_CONFIG;

void print_help() {
	std::cout << "~$ ./<executable name> <input file containing both s1 and s2> <0: global, 1: local> <optional: path to parameters config file>" << std::endl;
	std::cout << "E.g., ~$ ./align gene.fasta 0 parameters.config" << std::endl;
}

void format_bps(std::string & str) {
	for(auto& c : str) {
		c = tolower(c);
	}
}

std::vector<std::string> load_sequences(const char* fasta_fname) {
	std::cout << "Opening fasta file '" << fasta_fname << "' ..." << std::endl;
	std::ifstream fasta_stream(fasta_fname);
	if(!fasta_stream) {
		std::cout << "Failed to open input file '" << fasta_fname << "'." << std::endl;
		exit(1);
	}

	std::string line, sequence;
	std::vector<std::string> sequences;
	bool iterating_through_seq = false;
	while (std::getline(fasta_stream, line)) {
		if (line.empty()) {
			if (iterating_through_seq) {
				sequences.push_back(sequence);
				iterating_through_seq = false;
			}
		} else {
			// non empty
			if (line[0] == '>') {
				// found header
				std::cout << "Reading sequence: '" << line << "' ..." << std::endl;
				iterating_through_seq = true;
			} else {
				format_bps(line);
				sequence += line;
			}
		}
	}
	if (iterating_through_seq) {
		sequences.push_back(sequence);
	}

	if (sequences.size() != 2) {
		std::cout << "Improperly formatted input fasta file '" << fasta_fname << "'. Expected 2 sequences but found ";
		std::cout << sequences.size() << "." << std::endl;
		exit(1);
	}

	for(auto sequence : sequences) {
		//std::cout << "sequence: " << sequence << std::endl;
	}
	return sequences;
}

ALIGN_SCOPE parse_align_scope(const char *alignment_str) {
	enum ALIGN_SCOPE align_scope;
	switch(alignment_str[0]) {
	case '0':
		align_scope = GLOBAL;
		break;
	case '1':
		align_scope = LOCAL;
		break;
	default:
		std::cout << "Failed to parse alignment scope: '" << alignment_str << "' (should be 0 or 1)" << std::endl;
		exit(1);
	}
	return align_scope;
}

void print_score_config(SCORE_CONFIG & scores) {
	std::cout << "--Scoring configuration settings--" << std::endl;
	std::cout << "match: " << scores.match << std::endl;
	std::cout << "mismatch: " << scores.mismatch << std::endl;
	std::cout << "g: " << scores.g << std::endl;
	std::cout << "h: " << scores.h << std::endl;
}

SCORE_CONFIG load_config(const char *config_fname) {
	std::cout << "Loading configuration settings from file '" << config_fname << "' ..." << std::endl;
	std::ifstream config_stream(config_fname);
	if(!config_stream) {
		std::cout << "Failed to open config file '" << config_fname << "'." << std::endl;
		exit(1);
	}

	std::string line;
	SCORE_CONFIG scores;
	int provided = 0;
	while (std::getline(config_stream, line)) {
		// TODO use hashmap
		if (line.rfind("match ", 0) == 0) {
			provided |= 0b0001;
			int i = line.rfind(' ') + 1;
			scores.match = stoi(line.substr(i));
		} else if(line.rfind("mismatch ", 0) == 0) {
			provided |= 0b0010;
			int i = line.rfind(' ') + 1;
			scores.mismatch = stoi(line.substr(i));
		} else if(line.rfind("g ", 0) == 0) {
			provided |= 0b0100;
			int i = line.rfind(' ') + 1;
			scores.g = stoi(line.substr(i));
		} else if(line.rfind("h ", 0) == 0) {
			provided |= 0b1000;
			int i = line.rfind(' ') + 1;
			scores.h = stoi(line.substr(i));
		}
	}
	if (provided != 0b1111) {
		std::cout << "Warning: the parameter file '" << config_fname << "' did not provide values for each parameter (default value 0)" << std::endl;
	}
	print_score_config(scores);
	return scores;
}

int align_global(std::string & s1, std::string & s2, SCORE_CONFIG scores) {
	const size_t m = s1.size();
	const size_t n = s2.size();
	/*
				s2
		__________________
	   |
       |
	s1 |
	   |
	   |
	*/
	return 0;
}

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


	return 0;
}