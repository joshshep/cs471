#include <gtest/gtest.h>

#include "read-map/read-map.h"

TEST(ReadMap, Sample1) {
	std::string genome = "bcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzbcdefghijklmnopqrstuvwxyz";
	std::string read = "abcdefghijklmnopqrstuvwxyz";

	suffix_tree::Sequence genome_seq = {"genome", genome};
	suffix_tree::Sequence read_seq = {"read26", read};

	std::vector<read_map::Sequence> reads{read_seq};
	read_map::ReadMap * read_mapping = new read_map::ReadMap(genome_seq, reads);

	delete read_mapping;
}

TEST(ReadMap, Sample2) {
	std::string genome = "bcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzbcdefghijklmnopqrstuvwxyz";
	std::string read = "aaaaaaaaaaaa";

	suffix_tree::Sequence genome_seq = {"genome", genome};
	suffix_tree::Sequence read_seq = {"read12", read};
	auto st = new suffix_tree::SuffixTree(genome.c_str(), genome.size());

	std::vector<read_map::Sequence> reads{read_seq};
	read_map::ReadMap * read_mapping = new read_map::ReadMap(genome_seq, reads);

	delete read_mapping;
}
