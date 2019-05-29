#ifndef ALIGNER_H_
#define ALIGNER_H_

#include <string>
#include <climits> // INT_MIN
#include <iostream>     // std::cout
#include <algorithm>    // std::max
#include <fstream> 
#include <utility> // pair
#include <vector>
#include <assert.h>

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


// abstract class to determine the alignment between 2 base pair (BP) sequences 
class Aligner {
public:
	Aligner(const int s1_max_len, const int s2_max_len, const ScoreConfig & scoring) : 
		s1_max_len_(s1_max_len),
		s2_max_len_(s2_max_len),
		scoring_(scoring)
		{
		// alloc and initialize dp table
		AllocDP();
	}

	~Aligner() {
		// delete dp table
		DelDP();
	}

	// perform the alignment of the two sequences
	// optionally prints information about the alignment (this is mainly used for tests)
	// returms the alignment score (returned from the pure virtual RunDP())
	int Align(bool print_alignment=true);

	// sets the strings for the dp table comparisons
	void SetOperands(const char * s1, int s1_len, const char * s2, int s2_len) {
		assert(s1_len <= s1_max_len_);
		assert(s2_len <= s2_max_len_);
		s1_ = s1;
		s1_len_ = s1_len;
		s2_ = s2;
		s2_len_ = s2_len;
	}

protected:

	// (pure virtual) fills in the dp table with valid values
	virtual int RunDP() = 0;

	// (pure virtual) generates the alignment by backtracking through the dp table
	virtual Alignment RetraceDP() = 0;

	// complementary build-up/tear-down dp methods
	//  allocates the dp table
	void AllocDP();
	// deletes the dp table 
	void DelDP(); 

	// returns the cost to substitute c1 for c2 in the alignment
	int Cost2Sub(char c1, char c2);
	RetraceState GetRetraceState(const DP_Cell cell, char c_s1, char c_s2 );

	// prints assorted information about the determined alignment
	void PrintAlignStats(Alignment alignment);

	// prints the entire alignment 60 base pairs (or bp_per_line) per line
	// calls PrintAlignmentLine() every 60 base pairs in the alignment string
	void PrintAlignment(Alignment alignment, const int bp_per_line = 60);

	// prints a line of the alignment starting at i_retrace_start in the retrace string
	// helper function to PrintAlignment()
	void PrintAlignmentLine(const std::string & retrace, 
	                        const int i_retrace_start, int & i_s1, int & i_s2,
	                        const int bp_per_line);

	// vars
	DP_Cell** dp_; // the internal dp table we use to calculate the alignment
	const int s1_max_len_;
	const int s2_max_len_;
	const ScoreConfig & scoring_; // the loaded scores for aligning procedure
	

	const char * s1_ = nullptr;
	const char * s2_ = nullptr;
	int s1_len_ = -1;
	int s2_len_ = -1;
};


////// Utility free functions
// TODO: organize to match google style guide

////////////////////////////////
// printing 
////////////////////////////////
// prints the help to run this executable 
void PrintHelp();

// prints the scoring configuration values for {match, mismatch, gap start, gap extend}
void PrintScoreConfig(const ScoreConfig & scores);

// pretty-prints the size value as quantity of {B, KiB, MiB, GiB}
void PrintSize(size_t asize);


///////////////////////////////////////
// loading from files
///////////////////////////////////////
// loads exactly 2 sequences from the provided file name
std::pair<Sequence, Sequence> LoadSequences(const char* fasta_fname);

// loads the scoring configuration from the provided filename
ScoreConfig LoadConfig(const char *config_fname);



///////////////////////////////////////////
// misc free helpers (TODO)
///////////////////////////////////////////
// formats the input string (representing base pairs) to lowercase
void Format_bps(std::string & str);

// trims the provided whitespace characters from the provided string
std::string Trim(const std::string& str, const std::string& whitespace = " ");

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
