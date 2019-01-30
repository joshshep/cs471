#include "gtest/gtest.h"
#include "src/aligner.h"

TEST(AlignerTest, ToLowercase) {
	std::string s = "ATC";
	format_bps(s);
	EXPECT_EQ(s, std::string("atc"));
}
