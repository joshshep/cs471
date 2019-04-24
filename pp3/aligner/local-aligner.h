#ifndef LOCAL_ALIGNER_H_
#define LOCAL_ALIGNER_H_

#include "aligner/aligner.h"

namespace aligner {

typedef struct alignment_stats {
	int nMatches;
	int nMismatches;
	int nGaps;
	int startIndex;
} AlignmentStats;

// inherits Aligner to implement the Smith-Waterman algorithm
class LocalAligner final : public Aligner {
	// use base class's constructor
	using Aligner::Aligner;
	// the base class destructor is called automatically when localAligner is deleted

	// set the edge values in the dp table to zero
	void InitDP();

	// fills in the dp table using the Smith-Waterman algorithm for computing OPTIMAL LOCAL ALIGNMENT
	int RunDP() override;

	// determines the alignment by retracing/backtracking through the dp table
	Alignment RetraceDP() override;

	// determines the cell in the dp corresponding to the max val
	void MaxCellInDP(int &i_max, int &j_max);

public:
	// retraces without generating the retrace string
	// ... the stats are written to the input parameter scoring
	void CountRetraceStats(AlignmentStats & scoring);
};

} // namespace aligner
#endif
