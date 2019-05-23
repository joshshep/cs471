#include <gtest/gtest.h>

#include "read-map/read-map.h"

TEST(ReadMap, Sample1) {
	std::string genome = "bcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzbcdefghijklmnopqrstuvwxyz";
	std::string read = "abcdefghijklmnopqrstuvwxyz";

	suffix_tree::Sequence genome_seq = {"genome", genome};
	suffix_tree::Sequence read_seq = {"read26", read};
	auto st = new suffix_tree::SuffixTree(genome.c_str(), genome.size());

	std::vector<read_map::Sequence> reads{read_seq};
	const aligner::ScoreConfig align_config = {1, -2, -5, -1};
	read_map::ReadMap * read_mapping = new read_map::ReadMap(genome_seq, reads, align_config);
	int next_index = 0;
	read_mapping->PrepareST(st->root_, next_index);
	// TODO this is bad
	read_mapping->st_ = st;

	// TODO
	auto strpos = read_mapping->CalcReadMapping(read_seq);
	EXPECT_EQ(25, strpos.start);
	EXPECT_EQ(26, strpos.len);
	delete read_mapping;
	delete st;
}

TEST(ReadMap, Sample2) {
	std::string genome = "bcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzbcdefghijklmnopqrstuvwxyz";
	std::string read = "aaaaaaaaaaaa";

	suffix_tree::Sequence genome_seq = {"genome", genome};
	suffix_tree::Sequence read_seq = {"read12", read};
	auto st = new suffix_tree::SuffixTree(genome.c_str(), genome.size());

	std::vector<read_map::Sequence> reads{read_seq};
	const aligner::ScoreConfig align_config = {1, -2, -5, -1};
	read_map::ReadMap * read_mapping = new read_map::ReadMap(genome_seq, reads, align_config);
	int next_index = 0;
	read_mapping->PrepareST(st->root_, next_index);
	// TODO this is bad
	read_mapping->st_ = st;

	// TODO
	auto strpos = read_mapping->CalcReadMapping(read_seq);
	EXPECT_EQ(-1, strpos.start);
	EXPECT_EQ(-1, strpos.len);

	delete read_mapping;
	delete st;
}
