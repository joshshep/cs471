#include <gtest/gtest.h>

#include "read-map/read-map.h"

TEST(ReadMapWorker, Sample1) {
	std::string genome = "bcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzbcdefghijklmnopqrstuvwxyz";
	std::string read = "abcdefghijklmnopqrstuvwxyz";

	suffix_tree::Sequence genome_seq = {"genome", genome};
	suffix_tree::Sequence read_seq = {"read26", read};

	// build the suffix tree
	suffix_tree::SuffixTree* st = new suffix_tree::SuffixTree(genome_seq.bps.c_str(), genome_seq.bps.size());
	st->BuildTreeMccreight();

	// build A
	int next_index = 0;
	int * A = new int[genome_seq.bps.size()];
	read_map::ReadMap::PrepareST(A, st->root_, next_index);

	std::vector<read_map::Sequence> reads{read_seq};
	const aligner::ScoreConfig align_config = {1, -2, -5, -1};
	read_map::ReadMapWorker * worker = new read_map::ReadMapWorker(0, genome_seq, reads, align_config, *st, A);

	auto pos = worker->CalcReadMapping(read_seq);
	EXPECT_EQ(25, pos.start);
	EXPECT_EQ(26, pos.len);

	delete worker;
	delete[] A;
	delete st;
}

TEST(ReadMapWorker, Sample2) {
	std::string genome = "bcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzbcdefghijklmnopqrstuvwxyz";
	std::string read = "aaaaaaaaaaaa";

	suffix_tree::Sequence genome_seq = {"genome", genome};
	suffix_tree::Sequence read_seq = {"read12", read};

	// build the suffix tree
	suffix_tree::SuffixTree* st = new suffix_tree::SuffixTree(genome_seq.bps.c_str(), genome_seq.bps.size());
	st->BuildTreeMccreight();

	// build A
	int next_index = 0;
	int * A = new int[genome_seq.bps.size()];
	read_map::ReadMap::PrepareST(A, st->root_, next_index);

	std::vector<read_map::Sequence> reads{read_seq};
	const aligner::ScoreConfig align_config = {1, -2, -5, -1};
	read_map::ReadMapWorker * worker = new read_map::ReadMapWorker(0, genome_seq, reads, align_config, *st, A);

	auto pos = worker->CalcReadMapping(read_seq);
	EXPECT_EQ(-1, pos.start);
	EXPECT_EQ(-1, pos.len);

	delete worker;
	delete[] A;
	delete st;
}
