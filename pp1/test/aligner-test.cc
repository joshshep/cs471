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

