#ifndef READ_MAP_H_
#define READ_MAP_H_

#include "aligner/local-aligner.h"
#include "suffix-tree/suffix-tree.h"

#include <stdio.h>
#include <string.h>

class ReadMap {
	ReadMap(Sequence & genome, std::vector<Sequence> reads);

	int PrepareST(SuffixTreeNode* node);

	int Run();

	int genome_len_;
	const char * genome_bps_;
	int * A_;
	int next_index_;
	SuffixTree * st_;
};


#endif