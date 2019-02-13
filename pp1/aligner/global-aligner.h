#ifndef GLOBAL_ALIGNER_H_
#define GLOBAL_ALIGNER_H_

#include "aligner/aligner.h"

class GlobalAligner : public Aligner {
	// use base class's constructor
	using Aligner::Aligner;
	
	// fill in the dp table using the Needleman-Wunsch algorithm for computing OPTIMAL GLOBAL ALIGNMENT
	int RunDP();

	// determine the alignment by retracing/backtracking through the dp table
	Alignment RetraceDP();
};

#endif