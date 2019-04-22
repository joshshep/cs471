#ifndef READ_MAP_H_
#define READ_MAP_H_

#include <stdio.h>
#include <string.h>

#include "aligner/local-aligner.h"
#include "suffix-tree/suffix-tree.h"

namespace read_map {

// minimum exact match length
#define ZETA 25
#define MAX_READ_LEN 105
#define MIN_PROP_IDENTITY 0.90
#define MIN_PROP_LENGTH_COVERAGE 0.80

using suffix_tree::Sequence;

class ReadMap {
public:
	ReadMap(Sequence & genome, std::vector<Sequence>& reads);
	~ReadMap();

	int PrepareST(suffix_tree::SuffixTreeNode* node);

	int Run();

	suffix_tree::SuffixTreeNode* FindLoc(std::string & read);

	int CalcReadMapping(suffix_tree::Sequence & read);

	int CalcReadMappings();

	int Align(int genome_match_start, std::string & read);

	int genome_len_;
	const char * genome_bps_;
	int * A_;
	int next_index_;
	suffix_tree::SuffixTree * st_;
	Sequence & genome_;
	std::vector<Sequence> & reads_;
	aligner::LocalAligner * local_aligner_ = nullptr;
};

} // namespace read_map

#endif