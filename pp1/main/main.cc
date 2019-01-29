#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define MIN_N_ARGS (3)

void print_help() {
	std::cout << "~$ ./<executable name> <input file containing both s1 and s2> <0: global, 1: local> <optional: path to parameters config file>" << std::endl;
	std::cout << "E.g., ~$ ./align gene.fasta 0 parameters.config" << std::endl;
}

std::vector<std::string> load_sequences(const char* fasta_fname) {
	std::cout << "Opening input file '" << fasta_fname << "' ..." << std::endl;
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
				std::cout << "Found header: '" << line << "'." << std::endl;
				iterating_through_seq = true;
			} else {
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
		std::cout << "sequence: " << sequence << std::endl;
	}
	return sequences;
}

int main(int argc, char *argv[]) {
	if (argc < MIN_N_ARGS) {
		print_help();
		exit(1);
	}

	char * fasta_fname = argv[1];
	std::vector<std::string> sequences = load_sequences(fasta_fname);
	

	return 0;
}