#include "read-map/read-map-worker.h"

namespace read_map {

ReadMapWorker::ReadMapWorker(const int tid,
	const Sequence & genome,
	const std::vector<Sequence>& reads,
	const aligner::ScoreConfig& align_config, 
	const suffix_tree::SuffixTree& st, 
	const int * A)
	: tid_(tid), genome_(genome), reads_(reads), st_(st), A_(A) {
	genome_len_ = genome.bps.size();
	genome_bps_ = genome.bps.c_str();

	local_aligner_= new aligner::LocalAligner(MAX_READ_LEN * 2, MAX_READ_LEN, align_config);
}

ReadMapWorker::~ReadMapWorker() {
	delete local_aligner_;
}

SuffixTreeNode* ReadMapWorker::FindLoc(const std::string & read) {
	const char * read_bps = read.c_str();
	int read_len = read.size();

	SuffixTreeNode* longest_match_node = nullptr;
	SuffixTreeNode* search_src = st_.root_; // i.e., T
	int longest_match_len = ZETA - 1;
	assert(ZETA > 0);
	for (int i = 0; i < read_len - search_src->str_depth_ - ZETA + 1; i++) {
		int match_len = search_src->str_depth_;

		const char * query = read_bps + i + search_src->str_depth_;
		int query_len = read_len - i - search_src->str_depth_;
		assert(query_len > 0);
		auto cand_longest_match_node = search_src->MatchStr(query, query_len, match_len);
		assert(cand_longest_match_node);

		// check if this is the best node encountered thus far
		if (match_len > longest_match_len) {
			longest_match_len = match_len;
			longest_match_node = cand_longest_match_node;
		}

		// jump with our suffix link
		if (match_len == cand_longest_match_node->str_depth_ && cand_longest_match_node->suffix_link_) {
			// use this node's suffix link
			search_src = cand_longest_match_node->suffix_link_;
		} else {
			// use this node's parent's suffix link
			search_src = cand_longest_match_node->parent_->suffix_link_;
		}
		assert(search_src);
	}
	return longest_match_node;
}


void ReadMapWorker::FindLocSlow(const std::string & read, FixedHeap<FindLocCand>& deepest_nodes) {
	const char * read_bps = read.c_str();
	int read_len = read.size();

	SuffixTreeNode* search_src = st_.root_; // i.e., T
	assert(ZETA > 0);
	for (int i = 0; i < read_len - search_src->str_depth_ - ZETA + 1; i++) {
		int match_len = search_src->str_depth_;

		const char * query = read_bps + i + search_src->str_depth_;
		int query_len = read_len - i - search_src->str_depth_;
		assert(query_len > 0);
		auto cand_longest_match_node = search_src->MatchStr(query, query_len, match_len);
		assert(cand_longest_match_node);
		FindLocCand cand = {match_len, cand_longest_match_node};

		// push onto the min-heap (but only keep the top NUM_EXACT_MATCH_COMPS elements in the heap)
		deepest_nodes.fixed_push(cand);
		// jump with our suffix link
		if (match_len == cand_longest_match_node->str_depth_ && cand_longest_match_node->suffix_link_) {
			// use this node's suffix link
			search_src = cand_longest_match_node->suffix_link_;
		} else {
			// use this node's parent's suffix link
			search_src = cand_longest_match_node->parent_->suffix_link_;
		}
		assert(search_src);
	}
}


int ReadMapWorker::Align(int genome_align_start, const std::string & read, aligner::AlignmentStats & alignment_stats) {
	const char * read_bps = read.c_str();
	int read_len = read.size();

	int genome_align_len = read_len * 2;
	genome_align_len = std::min(genome_align_len, genome_len_ - genome_align_start);

	local_aligner_->SetOperands(genome_bps_ + genome_align_start, genome_align_len, read_bps, read_len);
	
	// perform the alignment
	int alignment_score = local_aligner_->Align(false);

	// gather stats about the alignment
	local_aligner_->CountRetraceStats(alignment_stats);

	n_aligns_++; // keep track of the number of times we have aligned reads
	return alignment_score;
}

Strpos ReadMapWorker::CalcReadMapping(const suffix_tree::Sequence & read) {
	// min-heap
	FixedHeap<FindLocCand> deepest_nodes([](FindLocCand a, FindLocCand b){
		return a.matchLen > b.matchLen;
	}, NUM_EXACT_MATCH_COMPS);
	FindLocSlow(read.bps, deepest_nodes);
	if (deepest_nodes.size() == 0 || deepest_nodes.top().matchLen < ZETA ) {
		//cout << "Warning: failed to find " << ZETA << " character exact match for read named '" << read.name << "'" << endl;
		return {-1, -1};
	}

	// iterate through each child node of this deepest_node
	int longest_align_len = -1;
	int read_len = read.bps.size();
	int read_map_loc = -1;
	int i_deepest_node = 0;
	while (!deepest_nodes.empty()) {
		auto find_loc_cand = deepest_nodes.top();
		deepest_nodes.pop();
		auto deepest_node = find_loc_cand.node;
		//printf("[%d] i_deepest_node=%d ; match_len=%d (%d nodes remaining)\n", tid_, i_deepest_node, find_loc_cand.matchLen, (int)deepest_nodes.size());
		++i_deepest_node;
		for (int leaf_index = deepest_node->start_leaf_index_; 
			leaf_index <= deepest_node->end_leaf_index_; 
			leaf_index++) {
			assert(leaf_index >= 0);
			int genome_match_start = A_[leaf_index];
			aligner::AlignmentStats alignment_stats;

			// we have the exact match location
			// now we need to offset this in order to perform the alignment
			int genome_align_start = genome_match_start - read_len;
			genome_align_start = std::max(genome_align_start, 0);

			// perform the alignment
			Align(genome_align_start, read.bps, alignment_stats);

			// handle the result
			int align_len = alignment_stats.nMatches + alignment_stats.nMismatches + alignment_stats.nGaps;
			double prop_identity = (double)alignment_stats.nMatches / align_len;
			double len_coverage = (double)align_len / read_len;

			if (prop_identity > MIN_PROP_IDENTITY && len_coverage > MIN_PROP_LENGTH_COVERAGE) {
				// good match
				if (align_len > longest_align_len) {
					// new best match
					longest_align_len = align_len;
					read_map_loc = genome_align_start + alignment_stats.startIndex;
				}
			}
		}
	}

	if (read_map_loc < 0) {
		//cout << "Failed to find a suitable alignment for '" << read.name << "'" << endl;
	}
	return {read_map_loc, longest_align_len};
}

void ReadMapWorker::CalcReadMappings(int start_index, int num_reads, std::vector<Strpos>& mappings) {
	int reads_mapped = 0;
	for (int i = start_index; i < start_index + num_reads; i++) {
		auto & read = reads_[i];
		if (reads_mapped % 5000 == 0) {
			printf("[%d] %d / %d reads mapped\n", tid_, reads_mapped, num_reads);
		}
		auto astrpos = CalcReadMapping(read);
		mappings[i] = astrpos;
		reads_mapped++;
	}
	printf("[%d] %d / %d reads mapped\n", tid_, num_reads, num_reads);
}

} // namespace read_map