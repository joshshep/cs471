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

#include "util.h"

namespace aligner {

// abstract class to determine the alignment between 2 base pair (BP) sequences 
class Aligner {
public:
	Aligner(const int s1_max_len, const int s2_max_len, const ScoreConfig & scoring);

	~Aligner();

	// perform the alignment of the two sequences
	// optionally prints information about the alignment (this is mainly used for tests)
	// returms the alignment score (returned from the pure virtual RunDP())
	int Align(bool print_alignment=true);

	// sets the strings for the dp table comparisons
	void SetOperands(const char * s1, int s1_len, const char * s2, int s2_len);

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

} //namespace aligner

#endif
