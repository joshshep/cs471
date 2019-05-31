#ifndef READ_MAP_H_
#define READ_MAP_H_

#include <thread>
#include "read-map/read-map-worker.h"

namespace read_map {

#define NUM_MAPPING_THREADS 4
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

class ReadMap {
public:
	ReadMap(Sequence & genome, std::vector<Sequence>& reads, const aligner::ScoreConfig & align_config)
		: genome_(genome), reads_(reads), align_config_(align_config) {
		
	}
	~ReadMap() {

	}

	// generates A
	// A contains 
	static void PrepareST(int *A, suffix_tree::SuffixTreeNode* node, int & next_index);

	std::vector<Strpos> LaunchThreads(const suffix_tree::SuffixTree& st, const int * A, int num_threads=8);

	void SaveMappings(std::string ofname, std::vector<Strpos>& mappings);

	void SaveMappingsStats(std::string ofname, std::vector<Strpos>& mappings);

	void Run(std::string ofname = "mapping-results.csv");

	Sequence & genome_;
	std::vector<Sequence> & reads_;
	const aligner::ScoreConfig & align_config_;

};

} // namespace read_map

#endif