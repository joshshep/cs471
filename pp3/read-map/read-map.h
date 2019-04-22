#ifndef READ_MAP_H_
#define READ_MAP_H_

#include <stdio.h>
#include <string.h>

#include "aligner/local-aligner.h"
#include "suffix-tree/suffix-tree.h"

namespace read_map {

using suffix_tree::Sequence;

class ReadMap {
public:
	ReadMap(Sequence & genome, std::vector<Sequence>& reads);

	int PrepareST(suffix_tree::SuffixTreeNode* node);

	int Run();

	suffix_tree::SuffixTreeNode* FindLoc(suffix_tree::Sequence & read);

	int CalcReadMapping(suffix_tree::Sequence & read);

	int CalcReadMappings();

	int Align(int genome_match_start, suffix_tree::Sequence & read);

	int genome_len_;
	const char * genome_bps_;
	int * A_;
	int next_index_;
	suffix_tree::SuffixTree * st_;
	Sequence & genome_;
	std::vector<Sequence> & reads_;
};

} // namespace read_map

#endif