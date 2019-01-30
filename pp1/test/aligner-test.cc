#include "gtest/gtest.h"
#include "src/aligner.h"

const SCORE_CONFIG dflt_scores = {
	.match = 1,
	.mismatch = -2,
	.h = -5,
	.g = -1
};

TEST(AlignGlobal, EmptyS1EmptyS2) {
	std::string s1 = "";
	std::string s2 = "";
	EXPECT_EQ(0, align_global(s1, s2, dflt_scores));
}

TEST(AlignGlobal, S1SameAsS2) {
	std::string s1 = "atcgc";
	std::string s2 = "atcgc";
	EXPECT_EQ(dflt_scores.match * s1.size(), align_global(s1, s2, dflt_scores));
}

TEST(AlignGlobal, Insert) {
	std::string s1 = "abcdefghi";
	std::string s2 = "zabcdefgh";
	EXPECT_EQ(3, align_global(s1, s2, dflt_scores));
}

TEST(FormatBps, UppercaseBps) {
	std::string s = "ATC";
	format_bps(s);
	EXPECT_EQ(s, std::string("atc"));
}

TEST(Cost2Sub, Match) {
	EXPECT_EQ(dflt_scores.match, cost2sub('a', 'a', dflt_scores));
}

TEST(Cost2Sub, Mismatch) {
	EXPECT_EQ(dflt_scores.mismatch, cost2sub('a', 't', dflt_scores));
}

TEST(IntLen, Pos) {
	EXPECT_EQ(1, int_len(1));
	EXPECT_EQ(1, int_len(9));

	EXPECT_EQ(2, int_len(10));
	EXPECT_EQ(2, int_len(42));
	EXPECT_EQ(2, int_len(99));

	EXPECT_EQ(5, int_len(10000));
	EXPECT_EQ(5, int_len(99999));

	EXPECT_EQ(10, int_len(2000000000));
}

TEST(IntLen, NonPos) {
	EXPECT_EQ(1, int_len(0));
	EXPECT_EQ(2, int_len(-1));

	EXPECT_EQ(2, int_len(-9));
	EXPECT_EQ(11, int_len(-1073741824));
}