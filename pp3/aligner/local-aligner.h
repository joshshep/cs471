#ifndef LOCAL_ALIGNER_H_
#define LOCAL_ALIGNER_H_

#include "aligner/aligner.h"

namespace aligner {

// inherits Aligner to implement the Smith-Waterman algorithm
class LocalAligner : public Aligner {
	// use base class's constructor
	using Aligner::Aligner;

	// set the edge values in the dp table to zero
	void InitDP();

	// fills in the dp table using the Smith-Waterman algorithm for computing OPTIMAL LOCAL ALIGNMENT
	int RunDP() override;

	// determines the alignment by retracing/backtracking through the dp table
	Alignment RetraceDP() override;

	// determines the cell in the dp corresponding to the max val
	void MaxCellInDP(int &i_max, int &j_max);
};

} // namespace aligner
#endif
