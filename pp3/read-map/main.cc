#include "read-map/read-map.h"
#include "read-map/util.h"

int main(int argc, char *argv[]) {
	// TODO where should I put these constants?
	const int kMinNumArgs = 3;
	const int kMaxNumArgs = 4;

	if (argc < kMinNumArgs) {
		std::cout << "Error: too few arguments" << std::endl;
		read_map::PrintHelp();
		exit(1);
	} else if (argc > kMaxNumArgs) {
		std::cout << "Error: too many arguments" << std::endl;
		read_map::PrintHelp();
		exit(1);
	}

	const char * genome_fname = argv[1];
	const char * reads_fname = argv[2];
	//const char * config_fname = argv[3];

	auto genome_vec = read_map::LoadSequencesVector(genome_fname);
	assert(genome_vec.size() == 1);
	auto genome = genome_vec[0];

	auto reads = read_map::LoadSequencesVector(reads_fname);

	//const auto config = aligner::LoadConfig(config_fname);
	const aligner::ScoreConfig align_config = {1, -2, -5, -1};
	auto mapping_ofname = read_map::GetOutputFilename(reads_fname);

	auto read_map = new read_map::ReadMap(genome, reads, align_config);
	read_map->Run(mapping_ofname);

	// delete read_map;

	return 0;
}
