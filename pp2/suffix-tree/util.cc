#include "suffix-tree/suffix-tree.h"


void PrintHelp() {
	std::cout << "~$ ./<executable name> <input file containing both s1 and s2> <0: global, 1: local> <optional: path to parameters config file>" << std::endl;
	std::cout << "E.g., ~$ ./align gene.fasta 0 params.config" << std::endl;
}

// modified from this s/o post https://stackoverflow.com/a/1798170
std::string Trim(const std::string& str,
                 const std::string& whitespace) {
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

void Format_bps(std::string & str) {
	for (auto& c : str) {
		c = tolower(c);
	}
}

Sequence LoadSequence(const char* fasta_fname) {
	std::cout << "Opening fasta file '" << fasta_fname << "' ..." << std::endl;
	std::ifstream fasta_stream(fasta_fname);
	if (!fasta_stream) {
		std::cout << "Failed to open input file '" << fasta_fname << "'." << std::endl;
		exit(1);
	}

	std::string line, seq_bps, seq_name;
	std::vector<Sequence> sequences;
	bool iterating_through_seq = false;
	while (std::getline(fasta_stream, line)) {
		if (line.empty()) {
			if (iterating_through_seq) {
				// end of sequence
				sequences.push_back({seq_name, seq_bps});
				seq_bps = "";
				seq_name = "";
				iterating_through_seq = false;
			}
		} else {
			// non empty
			if (line[0] == '>') {
				// found header
				// beginning of sequence
				line = Trim(line);
				seq_name = line;
				std::cout << "Reading sequence: '" << seq_name << "' ..." << std::endl;
				iterating_through_seq = true;
			} else {
				//iterating through sequence
				line = Trim(line);
				Format_bps(line);
				seq_bps += line;
			}
		}
	}
	if (iterating_through_seq) {
		sequences.push_back({seq_name, seq_bps});
	}

	if (sequences.size() != 1) {
		std::cout << "Improperly formatted input fasta file '" << fasta_fname << "'. Expected 2 sequences but found ";
		std::cout << sequences.size() << "." << std::endl;
		exit(1);
	}

	for(auto seq : sequences) {
		std::cout << "sequence bps: " << seq.bps << std::endl;
		std::cout << "sequence name: " << seq.name << std::endl;
	}
	return sequences[0];
}

void PrintSize(size_t asize) {
	if ((asize >> 30) > 0) {
		printf("%.2lf GiB", (double) asize / (1<<30));
	} else if ((asize >> 20) > 0) {
		printf("%.2lf MiB", (double) asize / (1<<20));
	} else if ((asize >> 10) > 0) {
		printf("%.2lf KiB", (double) asize / (1<<10));
	} else {
		printf("%d B", (int)asize);
	}
}

char uintLen(unsigned int i) {
	if (i >= 10) {
		return 1 + uintLen(i/10);
	}
	return 1;
}

char intLen(int i) {
	if (i < 0) {
		return 1 + uintLen(-i);
	}
	return uintLen(i);
}
