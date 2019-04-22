#include "gtest/gtest.h"

#include "read-map/read-map.h"


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
