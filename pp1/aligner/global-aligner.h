#ifndef GLOBAL_ALIGNER_H_
#define GLOBAL_ALIGNER_H_

#include "aligner/aligner.h"

// inherits Aligner to implement the Needleman-Wunsch algorithm
class GlobalAligner : public Aligner {
	// use base class's constructor
	using Aligner::Aligner;
	
	// set the Needleman-Wunsch algorithm edge values in the dp table
	void InitDP();

	// fill in the dp table using the Needleman-Wunsch algorithm for computing OPTIMAL GLOBAL ALIGNMENT
	int RunDP() override;

	// determine the alignment by retracing/backtracking through the dp table
	Alignment RetraceDP() override;
};

#endif