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
	ReadMap(Sequence & genome, std::vector<Sequence> reads);

	int PrepareST(suffix_tree::SuffixTreeNode* node);

	int Run();

	int genome_len_;
	const char * genome_bps_;
	int * A_;
	int next_index_;
	suffix_tree::SuffixTree * st_;
};

} // namespace read_map

#endif