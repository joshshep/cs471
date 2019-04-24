#ifndef READ_MAP_H_
#define READ_MAP_H_

#include <stdio.h>
#include <string.h>
#include <chrono>

#include "aligner/local-aligner.h"
#include "suffix-tree/suffix-tree.h"

namespace read_map {

// minimum exact match length
#define ZETA 25
#define MAX_READ_LEN 105
#define MIN_PROP_IDENTITY 0.90
#define MIN_PROP_LENGTH_COVERAGE 0.80

typedef struct strpos {
	int start;
	int len;
} Strpos;

using suffix_tree::Sequence;
using std::cout;
using std::endl;

class ReadMap {
public:
	ReadMap(Sequence & genome, std::vector<Sequence>& reads, const aligner::ScoreConfig& align_config);
	~ReadMap();

	int PrepareST(suffix_tree::SuffixTreeNode* node);

	int Run(std::string ofname = "mapping-results.csv");

	suffix_tree::SuffixTreeNode* FindLoc(std::string & read);

	Strpos CalcReadMapping(suffix_tree::Sequence & read);

	std::vector<Strpos> CalcReadMappings();

	int Align(int genome_align_start, std::string & read, aligner::AlignmentStats & alignment_stats);

	void SaveMappingsStats(std::string ofname, std::vector<Strpos>& mappings);
	
	void SaveMappings(std::string ofname, std::vector<Strpos>& mappings);

	int genome_len_;
	const char * genome_bps_;
	int * A_;
	int next_index_;
	suffix_tree::SuffixTree * st_;
	Sequence & genome_;
	std::vector<Sequence> & reads_;
	aligner::LocalAligner * local_aligner_ = nullptr;
	int n_aligns_ = 0;
};

} // namespace read_map

#endif