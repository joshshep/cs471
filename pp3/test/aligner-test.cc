#include "gtest/gtest.h"
#include "aligner/local-aligner.h"

using aligner::ScoreConfig;
using aligner::Sequence;
using aligner::LocalAligner;
using aligner::intLen;
using aligner::Format_bps;

// TODO don't use macros
#define PRINT_ALIGNMENT false
const ScoreConfig dflt_scoring = {
	.match = 1,
	.mismatch = -2,
	.h = -5,
	.g = -1
};

/////////////////////////////////////
// LocalAlign tests
/////////////////////////////////////
TEST(LocalAlign, EmptyS1EmptyS2) {
	Sequence s1 = {"empty str 1", ""};
	Sequence s2 = {"empty str 2", ""};
	LocalAligner aligner(s1.bps.size(), s2.bps.size(), dflt_scoring);
	aligner.SetOperands(s1.bps.c_str(), s1.bps.size(), s2.bps.c_str(), s2.bps.size());
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(0, align_score);
}

TEST(LocalAlign, S1SameAsS2) {
	Sequence s1 = {"s1", "atcgc"};
	Sequence s2 = {"s2", "atcgc"};
	LocalAligner aligner(s1.bps.size(), s2.bps.size(), dflt_scoring);
	aligner.SetOperands(s1.bps.c_str(), s1.bps.size(), s2.bps.c_str(), s2.bps.size());
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	int expected_score = dflt_scoring.match * s1.bps.size();
	EXPECT_EQ(expected_score, align_score);
}

TEST(LocalAlign, SingleInsert) {
	Sequence s1 = {"s1", "abcdefghi"};
	Sequence s2 = {"z + s1", "zabcdefghi"};
	LocalAligner aligner(s1.bps.size(), s2.bps.size(), dflt_scoring);
	aligner.SetOperands(s1.bps.c_str(), s1.bps.size(), s2.bps.c_str(), s2.bps.size());
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(9 * dflt_scoring.match, align_score);
}

TEST(LocalAlign, MismatchSandwich) {
	Sequence s1 = {"s1", "abbbbbbbba"};
	Sequence s2 = {"s2", "acccccccca"};
	LocalAligner aligner(s1.bps.size(), s2.bps.size(), dflt_scoring);
	aligner.SetOperands(s1.bps.c_str(), s1.bps.size(), s2.bps.c_str(), s2.bps.size());
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(dflt_scoring.match, align_score);
}

TEST(LocalAlign, GapSandwich) {
	Sequence s1 = {"s1", "aa"};
	Sequence s2 = {"s2", "abbbbbbba"};
	LocalAligner aligner(s1.bps.size(), s2.bps.size(), dflt_scoring);
	aligner.SetOperands(s1.bps.c_str(), s1.bps.size(), s2.bps.c_str(), s2.bps.size());
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(dflt_scoring.match, align_score);
}

TEST(LocalAlign, cs471sample) {
	const std::string s1_bps = "acatgctacacgtatccgataccccgtaaccgataacgatacacagacctcgtacgcttgctacaacgtactctataaccgagaacgattgacatgcctcgtacacatgctacacgtactccgat";
	const std::string s2_bps = "acatgcgacactactccgataccccgtaaccgataacgatacagagacctcgtacgcttgctaataaccgagaacgattgacattcctcgtacagctacacgtactccgat";
	const ScoreConfig params = {
		.match = 1,
		.mismatch = -2,
		.h = -5,
		.g = -2
	};
	Sequence s1 = {"s1", s1_bps};
	Sequence s2 = {"s2", s2_bps};
	LocalAligner aligner(s1.bps.size(), s2.bps.size(), params);
	aligner.SetOperands(s1.bps.c_str(), s1.bps.size(), s2.bps.c_str(), s2.bps.size());
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(55, align_score);
}

TEST(LocalAlign, Case0) {
	Sequence s1 = {"s1", "aaaaaaaaaaaaaaaaaabcbcbcbcbcbcaaaabcbcbcbcbcbcbcaaaaaaaaaaaaaaaa"};
	Sequence s2 = {"s2", "ddddbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbceeeeee"};
	LocalAligner aligner(s1.bps.size(), s2.bps.size(), dflt_scoring);
	aligner.SetOperands(s1.bps.c_str(), s1.bps.size(), s2.bps.c_str(), s2.bps.size());
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(18, align_score);
}

//////////////////////////////////////////////
// Misc. tests
//////////////////////////////////////////////
TEST(FormatBps, UppercaseBps) {
	std::string s = "ATC";
	Format_bps(s);
	EXPECT_EQ(s, std::string("atc"));
}

TEST(IntLen, Pos) {
	EXPECT_EQ(1, intLen(1));
	EXPECT_EQ(1, intLen(9));

	EXPECT_EQ(2, intLen(10));
	EXPECT_EQ(2, intLen(42));
	EXPECT_EQ(2, intLen(99));

	EXPECT_EQ(5, intLen(10000));
	EXPECT_EQ(5, intLen(99999));

	EXPECT_EQ(10, intLen(2000000000));
}

TEST(IntLen, NonPos) {
	EXPECT_EQ(1, intLen(0));
	EXPECT_EQ(2, intLen(-1));

	EXPECT_EQ(2, intLen(-9));
	EXPECT_EQ(11, intLen(-1073741824));
}
