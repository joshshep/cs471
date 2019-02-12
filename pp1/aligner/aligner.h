// TODO: rename SCORE_CONFIG and DP_CELL
// TODO: virtual var (for alignment type string)?
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
	std::string retrace_str;
	int s1_start;
	int s2_start;
} Alignment;

enum RETRACE_STATE { INSERT = 'i', DELETE = 'd', MATCH = '|', MISMATCH = 'X'};
enum AlignmentScope {GLOBAL, LOCAL};

class Aligner
{
public:
	Aligner(const std::string & s1, const std::string & s2, const ScoreConfig & scoring) : s1_(s1), s2_(s2), scoring_(scoring)
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

	// pure virtual
	// TODO
	//virtual int RunDP() = 0;
	int RunDP();
	//virtual Alignment RetraceDP() = 0;
	Alignment RetraceDP();

	// complementary build-up/tear-down dp methods
	virtual void InitDP();
	void DelDP();

	int Cost2Sub(char c1, char c2);

	// prints
	void PrintAlignStats(Alignment alignment);
	void PrintAlignment(Alignment alignment, const int bp_per_line = 60);
	void PrintAlignmentLine(Alignment alignment, 
						const int i_retrace_start, int & i_s1, int & i_s2,
                        const int bp_per_line);

	// vars
	DP_Cell** dp_;
	const std::string & s1_;
	const std::string & s2_;
	const ScoreConfig & scoring_;
};


// TODO: organize/rename to match google style guide
void print_help();
void format_bps(std::string & str);
std::string trim(const std::string& str, const std::string& whitespace = " ");
std::pair<std::string, std::string> load_sequences(const char* fasta_fname);
AlignmentScope parse_align_scope(const char *alignment_str);
void print_score_config(const ScoreConfig & scores);
ScoreConfig load_config(const char *config_fname);
void print_size(size_t asize);
char int_len(int n);
int max3(int &i0, int &i1, int &i2);
int max3(DP_Cell & a);

#endif