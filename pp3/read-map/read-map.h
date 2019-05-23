#ifndef READ_MAP_H_
#define READ_MAP_H_

#include "read-map/read-map-worker.h"

namespace read_map {

class ReadMap {
public:
	ReadMap(Sequence & genome, std::vector<Sequence>& reads) : genome_(genome), reads_(reads) {
		
	}
	~ReadMap() {
		
	}

	// generates A
	void PrepareST(int *A, suffix_tree::SuffixTreeNode* node, int & next_index);

	void Run() {
		// generate st
		const suffix_tree::SuffixTree * st = new suffix_tree::SuffixTree(genome_.bps.c_str(),  genome_.bps.size());

		// generate A
		int next_index = 0;
		int * A = new int[genome_.bps.size()];
		PrepareST(A, st->root_, next_index);
		
		// do the work
		const aligner::ScoreConfig align_config = {1, -2, -5, -1};
		auto worker = new ReadMapWorker(genome_, reads_, align_config, *st, A);
		auto locs = worker->CalcReadMappings();

		// output the results
		for (auto loc : locs) {
			cout << "(" << loc.start << "," << loc.len << ")" << endl;
		}

		delete worker;
		delete[] A;
		delete st;
	}

	Sequence & genome_;
	std::vector<Sequence> & reads_;
};

} // namespace read_map

#endif