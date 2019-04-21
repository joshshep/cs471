#ifndef READ_MAP_H_
#define READ_MAP_H_

#include "aligner/local-aligner.h"
#include "suffix-tree/suffix-tree.h"

#include <stdio.h>

int ReadMap(Sequence & genome, std::vector<Sequence> reads);


#endif