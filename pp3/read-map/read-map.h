#ifndef READ_MAP_H_
#define READ_MAP_H_

#include <stdio.h>
#include <string.h>
#include <chrono>

#include "aligner/local-aligner.h"
#include "suffix-tree/suffix-tree.h"

namespace read_map {

// minimum exact match length
#define ZETA 25
#define MAX_READ_LEN 105
#define MIN_PROP_IDENTITY 0.90
#define MIN_PROP_LENGTH_COVERAGE 0.80

typedef struct strpos {
	int start;
	int len;
} Strpos;

using suffix_tree::Sequence;
using std::cout;
using std::endl;

class ReadMap {
public:
	ReadMap(Sequence & genome, std::vector<Sequence>& reads, const aligner::ScoreConfig& align_config);
	~ReadMap();

	// generates A
	void PrepareST(suffix_tree::SuffixTreeNode* node, int & next_index);

	// runs all of the steps in mapping the reads to the reference sequence "genome"
	void Run(std::string ofname = "mapping-results.csv");

	// finds the location to which this read maps in the reference sequnece
	// returns the suffix tree node corresponding to the location 
	suffix_tree::SuffixTreeNode* FindLoc(std::string & read);

	// wraps FindLoc() and Align() for this read
	// returns the location on the reference genome to which this read was aligned
	Strpos CalcReadMapping(suffix_tree::Sequence & read);

	// wraps CalcReadMapping()
	// returns all of the locations of the mapped reads
	std::vector<Strpos> CalcReadMappings();

	// returns the alignment score between the read and the reference sequence at position genome_align_start
	int Align(int genome_align_start, std::string & read, aligner::AlignmentStats & alignment_stats);

	// writes the mapping locations for each read to the output file
	// note: the additional stats require that the read name contains the read position
	void SaveMappingsStats(std::string ofname, std::vector<Strpos>& mappings);
	
	// writes the mapping locations for each read to the output file
	void SaveMappings(std::string ofname, std::vector<Strpos>& mappings);

	int genome_len_;
	const char * genome_bps_;
	int * A_ = nullptr;
	suffix_tree::SuffixTree * st_;
	Sequence & genome_;
	std::vector<Sequence> & reads_;
	aligner::LocalAligner * local_aligner_ = nullptr;
	int n_aligns_ = 0;
};

} // namespace read_map

#endif