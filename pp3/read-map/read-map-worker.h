#ifndef READ_MAP_WORKER_H_
#define READ_MAP_WORKER_H_

#include <stdio.h>
#include <string.h>
#include <chrono>

#include "read-map/fixed-heap.h"
#include "aligner/local-aligner.h"
#include "suffix-tree/suffix-tree.h"

namespace read_map {

// minimum exact match length
#define ZETA 25
#define MAX_READ_LEN 105
#define MIN_PROP_IDENTITY 0.90
#define MIN_PROP_LENGTH_COVERAGE 0.80
#define NUM_EXACT_MATCH_COMPS 5

using suffix_tree::Sequence;
using suffix_tree::SuffixTreeNode;
using std::cout;
using std::endl;

typedef struct strpos {
	int start;
	int len;
} Strpos;

/* 
a record of a ST node and its corresponding match length
this is used to keep track of candidates in the exaxt match heap
*/
typedef struct findLocCand {
	int matchLen;
	SuffixTreeNode* node;
}FindLocCand;

class ReadMapWorker {
public:
	ReadMapWorker(
		const int tid,
		const Sequence & genome,
		const std::vector<Sequence>& reads,
		const aligner::ScoreConfig& align_config,
		const suffix_tree::SuffixTree& st,
		const int * A);
	~ReadMapWorker();

	// finds the location to which this read maps in the reference sequnece
	// returns the suffix tree node corresponding to the location 
	SuffixTreeNode* FindLoc(const std::string & read);
	auto FindLocSlow(const std::string & read);

	// returns the alignment score between the read and the reference sequence at position genome_align_start
	int Align(int genome_align_start, const std::string & read, aligner::AlignmentStats & alignment_stats);

	// wraps FindLoc() and Align() for this read
	// returns the location on the reference genome to which this read was aligned
	Strpos CalcReadMapping(const suffix_tree::Sequence & read);

	// wraps CalcReadMapping()
	// returns all of the locations of the mapped reads
	void CalcReadMappings(int start_index, int num_reads, std::vector<Strpos>& mappings);

	const int tid_;
	const Sequence & genome_;
	const std::vector<Sequence> & reads_;
	const suffix_tree::SuffixTree & st_;
	const int * A_ = nullptr;
	aligner::LocalAligner * local_aligner_ = nullptr;
	int n_aligns_ = 0;

	const char * genome_bps_;
	int genome_len_;
};

} // namespace read_map

#endif