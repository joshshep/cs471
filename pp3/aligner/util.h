#ifndef ALIGNER_UTIL_H_
#define ALIGNER_UTIL_H_

#include <iostream>
#include <fstream>
#include <algorithm> // std::max


#include "aligner/types.h"

namespace aligner {


////// Utility free functions

////////////////////////////////
// printing 
////////////////////////////////

// prints the scoring configuration values for {match, mismatch, gap start, gap extend}
void PrintScoreConfig(const ScoreConfig & scores);

// pretty-prints the size value as quantity of {B, KiB, MiB, GiB}
void PrintSize(size_t asize);

///////////////////////////////////////////
// misc free helpers (TODO)
///////////////////////////////////////////

// returns the alignment scope (local or global) given a command line argument string
AlignmentScope ParseAlignScope(const char *alignment_str);

// determines the length of the input integer
char intLen(int n);

// returns the maximum of the values in the dp cell
int max3(const DP_Cell & a);

// prints the dp table using ridiculously tedious and complicated ascii art
void PrintDP_Table(DP_Cell** dp, const std::string & s1, const std::string & s2);

} //namespace aligner

#endif
