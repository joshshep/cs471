#include "read-map/read-map.h"

#include <assert.h>
#include <libgen.h>

void PrintHelp() {
	std::cout << "~$ ./read-map <FASTA file containing reference genome sequence G> <FASTA file containing reads> <input alphabet file>" << std::endl;
	std::cout << "E.g., ~$ ./bazel-bin/read-map/read-map data/Peach_reference.fasta data/Peach_simulated_reads.fasta data/alpha.txt" << std::endl;
}

// modified from this s/o post https://stackoverflow.com/a/1798170
std::string Trim(const std::string& str,
                 const std::string& delimiter = " ") {
	const auto strBegin = str.find_first_not_of(delimiter);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(delimiter);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

void Format_bps(std::string & str) {
	for (auto& c : str) {
		c = tolower(c);
	}
}

std::vector<read_map::Sequence> LoadSequencesVector(const char* fasta_fname) {
	std::cout << "Opening fasta file '" << fasta_fname << "' ..." << std::endl;
	std::ifstream fasta_stream(fasta_fname);
	if (!fasta_stream) {
		std::cout << "Failed to open input file '" << fasta_fname << "'." << std::endl;
		exit(1);
	}

	std::string line, seq_bps, seq_name;
	std::vector<suffix_tree::Sequence> sequences;
	bool iterating_through_seq = false;
	while (std::getline(fasta_stream, line)) {
		if (line.empty()) {
			continue;
		}
		// non empty
		if (line[0] == '>') {
			// found header
			// beginning of sequence
			if (iterating_through_seq) {
				// end of previous sequence too!
				sequences.push_back({seq_name, seq_bps});
			}
			line = Trim(line);
			seq_bps = "";
			seq_name = line;
			//std::cout << "Reading sequence: '" << seq_name << "' ..." << std::endl;
			iterating_through_seq = true;
		} else {
			//iterating through sequence
			line = Trim(line);
			Format_bps(line);
			seq_bps += line;
		}
	}
	if (iterating_through_seq) {
		sequences.push_back({seq_name, seq_bps});
	}

	for (auto seq : sequences) {
		//std::cout << "sequence bps: " << seq.bps << std::endl;
		//std::cout << "sequence name: " << seq.name << std::endl;
	}
	std::cout << "Loaded " << sequences.size() << " fasta sequence(s)" << std::endl;
	return sequences;
}

void StripExt(char *s) {
	while (*s) {
		if (*s == '.') {
			*s = 0;
			return;
		}
		s++;
	}
}

std::string GetOutputFilename(const char * reads_fname) {
	char bname_buf[1024];
	strcpy(bname_buf, reads_fname);
	char *bname = basename(bname_buf);
	StripExt(bname);

	std::string mapping_ofname("mapping-results-");
	mapping_ofname += bname;
	mapping_ofname += ".csv";

	return mapping_ofname;
}

int main(int argc, char *argv[]) {
	// TODO where should I put these constants?
	const int kMinNumArgs = 3;
	const int kMaxNumArgs = 4;

	if (argc < kMinNumArgs) {
		std::cout << "Error: too few arguments" << std::endl;
		PrintHelp();
		exit(1);
	} else if (argc > kMaxNumArgs) {
		std::cout << "Error: too many arguments" << std::endl;
		PrintHelp();
		exit(1);
	}

	const char * genome_fname = argv[1];
	const char * reads_fname = argv[2];
	//const char * config_fname = argv[3];

	auto genome_vec = LoadSequencesVector(genome_fname);
	assert(genome_vec.size() == 1);
	auto genome = genome_vec[0];

	auto reads = LoadSequencesVector(reads_fname);

	//const auto config = aligner::LoadConfig(config_fname);
	const aligner::ScoreConfig align_config = {1, -2, -5, -1};
	auto mapping_ofname = GetOutputFilename(reads_fname);

	auto read_map = new read_map::ReadMap(genome, reads, align_config);
	read_map->Run(mapping_ofname);

	// delete read_map;

	return 0;
}
