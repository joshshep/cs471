#ifndef READ_MAP_WORKER_H_
#define READ_MAP_WORKER_H_

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

class ReadMapWorker {
public:
	ReadMapWorker(const int tid, const Sequence & genome, const std::vector<Sequence>& reads, const aligner::ScoreConfig& align_config, const suffix_tree::SuffixTree& st, const int * A);
	~ReadMapWorker();

	// runs all of the steps in mapping the reads to the reference sequence "genome"
	void Run(std::string ofname = "mapping-results.csv");

	// finds the location to which this read maps in the reference sequnece
	// returns the suffix tree node corresponding to the location 
	suffix_tree::SuffixTreeNode* FindLoc(const std::string & read);

	// wraps FindLoc() and Align() for this read
	// returns the location on the reference genome to which this read was aligned
	Strpos CalcReadMapping(const suffix_tree::Sequence & read);

	// wraps CalcReadMapping()
	// returns all of the locations of the mapped reads
	std::vector<Strpos> CalcReadMappings(int start_index, int num_reads);

	// returns the alignment score between the read and the reference sequence at position genome_align_start
	int Align(int genome_align_start, const std::string & read, aligner::AlignmentStats & alignment_stats);

	// writes the mapping locations for each read to the output file
	// note: the additional stats require that the read name contains the read position
	void SaveMappingsStats(std::string ofname, std::vector<Strpos>& mappings);
	
	// writes the mapping locations for each read to the output file
	void SaveMappings(std::string ofname, std::vector<Strpos>& mappings);

	const int tid_;
	const Sequence & genome_;
	const suffix_tree::SuffixTree & st_;
	const std::vector<Sequence> & reads_;
	const int * A_ = nullptr;
	aligner::LocalAligner * local_aligner_ = nullptr;
	int n_aligns_ = 0;

	const char * genome_bps_;
	int genome_len_;
};

} // namespace read_map

#endif