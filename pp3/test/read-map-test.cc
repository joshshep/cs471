#include <gtest/gtest.h>

#include "read-map/read-map.h"

/*
TEST(ReadMap, Sample1) {
	std::string genome = "bcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzbcdefghijklmnopqrstuvwxyz$";
	std::string read = "abcdefghijklmnopqrstuvwxyz";

	read_map::Sequence genome_seq = {"genome", genome};
	read_map::Sequence read_seq = {"read25", read};
	std::vector<read_map::Sequence> reads{read_seq};
	read_map::ReadMap * read_mapping = new read_map::ReadMap(genome_seq, reads);
	read_mapping->Run();
	EXPECT_EQ(0, 0);
}
*/

/*
TEST(SuffixTree, Sample1) {
	std::string genome = "banana$";
	std::string read = "ana";

	int match_len = 0;
	suffix_tree::SuffixTree* st = new suffix_tree::SuffixTree(genome.c_str(), genome.size());
	auto match_node = st->root_->MatchStr(read.c_str(), read.size(), match_len);
	printf("match_len = %d\n", match_len);
	st->PrintPath(match_node);
	printf("\n");
	st->PrintTree();
	st->root_->PrintParentage();
	delete st;
	EXPECT_EQ(0, 0);
}
*/
