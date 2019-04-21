#include "read-map/read-map.h"

std::vector<Sequence> LoadSequencesVector(const char* fasta_fname) {
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

	for(auto seq : sequences) {
		std::cout << "sequence bps: " << seq.bps << std::endl;
		std::cout << "sequence name: " << seq.name << std::endl;
	}
	return sequnces;
}

int main(int argc, char *argv[]) {
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

	const char * genome = argv[1];
	const char * reads_fname = argv[2];
	auto genome_vec = LoadSequencesVector(reads_fname);
	assert(genome_vec.length == 1);
	auto genome = genome_vec[0];

	auto reads = LoadSequencesVector(reads_fname);
	printf("Read %d read(s) from fname '%s'\n", reads.length, reads_fname);

	ReadMap* read_map = new ReadMap(genome, reads);

	read_map->Run();

	return 0;
}