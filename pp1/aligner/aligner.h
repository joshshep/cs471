#ifndef ALIGNER_H_
#define ALIGNER_H_

#include <string>
#include <climits>
#include <iostream>     // std::cout
#include <algorithm>    // std::max
#include <fstream>  // reading from files
#include <utility> // pair
#include <vector>


typedef struct dp_cell {
	int S;
	int D;
	int I;
} DP_Cell;

typedef struct score_config {
	int match;
	int mismatch;
	int h; // start gap penalty
	int g; // continuing gap penalty
} ScoreConfig;

typedef struct alignment {
	std::string retrace;
	int s1_start;
	int s2_start;
} Alignment;

typedef struct sequence {
	std::string bps;
	std::string name;
} Sequence;

enum RetraceState { INSERT = 'i', DELETE = 'd', MATCH = '|', MISMATCH = 'X'};
enum AlignmentScope {GLOBAL, LOCAL};


// abstract class to determine the alignment between 2 base pair (BP) sequences 
class Aligner
{
public:
	// TODO: resolve redundancy in {s1, s2} and seqs without messing with non-POD issues in initialization lists
	Aligner(const std::string & s1, const std::string & s2, const std::pair<Sequence, Sequence> & seqs, const ScoreConfig & scoring) : 
		s1_(s1),
		s2_(s2),
		seqs_(seqs),
		scoring_(scoring)
	{
		// alloc and initialize dp table
		InitDP();
	}

	~Aligner()
	{
		// delete dp table
		DelDP();
	}

	int Align(bool print_alignment=true);

protected:

	// (pure virtual) fills in the dp table with valid values
	virtual int RunDP() = 0;

	// (pure virtual) generates the alignment by backtracking through the dp table
	virtual Alignment RetraceDP() = 0;

	// complementary build-up/tear-down dp methods
	// (virtual) allocates and initializes the dp table with edge values
	virtual void InitDP();

	// deletes the dp table
	void DelDP();

	// returns the cost to substitute c1 for c2 in the alignment
	int Cost2Sub(char c1, char c2);
	RetraceState GetRetraceState(const DP_Cell cell, char c_s1, char c_s2 );

	// prints assorted information about the determined alignment
	void PrintAlignStats(Alignment alignment);

	// prints the entire alignment 60 base pairs (or bp_per_line) per line
	void PrintAlignment(Alignment alignment, const int bp_per_line = 60);

	// prints a line of the alignment starting at i_retrace_start in the retrace string
	void PrintAlignmentLine(const std::string & retrace, 
	                        const int i_retrace_start, int & i_s1, int & i_s2,
	                        const int bp_per_line);

	// vars
	
	DP_Cell** dp_; // the internal dp table we use to calculate the alignment
	const std::string & s1_; // string representing base pair sequence 1
	const std::string & s2_; // string representing base pair sequence 2
	const std::pair<Sequence, Sequence> & seqs_; // pair of sequences to align (TODO redundant)
	const ScoreConfig & scoring_; // the loaded scores for aligning procedure
};


////// Utility free functions
// TODO: organize/rename to match google style guide

// prints the help to run this executable 
void PrintHelp();

// formats the input string (representing base pairs) to lowercase
void Format_bps(std::string & str);

// trims the provided whitespace characters from the provided string
std::string Trim(const std::string& str, const std::string& whitespace = " ");

// loads exactly 2 sequences from the provided file name
std::pair<Sequence, Sequence> LoadSequences(const char* fasta_fname);

// returns the alignment scope (local or global) given a command line argument string
AlignmentScope ParseAlignScope(const char *alignment_str);

// prints the scoring configuration values for {match, mismatch, gap start, gap extend}
void PrintScoreConfig(const ScoreConfig & scores);

// loads the scoring configuration from the provided filename
ScoreConfig LoadConfig(const char *config_fname);

// pretty-prints the size value as quantity of {B, KiB, MiB, GiB}
void PrintSize(size_t asize);

// determines the length of the input integer
char intLen(int n);

// returns the maximum of the 3 input values
inline int max3(const int i0, const int i1, const int i2);

// returns the maximum of the values in the dp cell
inline int max3(const DP_Cell & a);

// prints the dp table using ridiculously tedious and complicated ascii art
void PrintDP_Table(DP_Cell** dp, const std::string & s1, const std::string & s2);

#endif