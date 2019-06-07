#ifndef ALIGNER_TYPES_H_
#define ALIGNER_TYPES_H_

#include <string>

namespace aligner {

typedef struct dp_cell {
	int D;
	int I;
	int S;
} DP_Cell;

typedef struct score_config {
	int match; // match reward (generally positive)
	int mismatch; // mismatch penalty (generally negative)
	int h; // start gap penalty (generally negative)
	int g; // continuing gap penalty (generally negative)
} ScoreConfig;

typedef struct alignment {
	std::string retrace;
	int s1_start;
	int s2_start;
} Alignment;

typedef struct sequence {
	std::string name;
	std::string bps;
} Sequence;

enum RetraceState { INSERT = 'i', DELETE = 'd', MATCH = '|', MISMATCH = 'X'};
enum AlignmentScope {GLOBAL, LOCAL};

} //namespace aligner

#endif
