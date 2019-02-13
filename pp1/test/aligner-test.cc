#include "gtest/gtest.h"
#include "aligner/local-aligner.h"
#include "aligner/global-aligner.h"


// TODO don't use macros
#define PRINT_ALIGNMENT false
const ScoreConfig dflt_params = {
	.match = 1,
	.mismatch = -2,
	.h = -5,
	.g = -1
};

/////////////////////////////////////
// GlobalAlign tests
/////////////////////////////////////
TEST(GlobalAlign, EmptyS1EmptyS2) {
	Sequence s1 = {"empty str 1", ""};
	Sequence s2 = {"empty str 2", ""};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(0, align_score);
}

TEST(GlobalAlign, S1SameAsS2) {
	Sequence s1 = {"s1", "atcgc"};
	Sequence s2 = {"s2", "atcgc"};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	int expected_score = dflt_params.match * s1.bps.size();
	EXPECT_EQ(expected_score, align_score);
}

TEST(GlobalAlign, SingleInsert) {
	Sequence s1 = {"s1", "abcdefghi"};
	Sequence s2 = {"z + s1", "zabcdefghi"};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(3, align_score);
}

TEST(GlobalAlign, MismatchSandwich) {
	Sequence s1 = {"s1", "abbbbbbbba"};
	Sequence s2 = {"s2", "acccccccca"};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(-14, align_score);
}

TEST(GlobalAlign, GapSandwich) {
	Sequence s1 = {"s1", "aa"};
	Sequence s2 = {"s2", "abbbbbbba"};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(-10, align_score);
}

TEST(GlobalAlign, cs471sample) {
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
	GlobalAligner aligner(s1_bps, s2_bps, std::pair<Sequence, Sequence>(s1, s2), params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(55, align_score);
}



/////////////////////////////////////
// LocalAlign tests
/////////////////////////////////////
TEST(LocalAlign, EmptyS1EmptyS2) {
	Sequence s1 = {"empty str 1", ""};
	Sequence s2 = {"empty str 2", ""};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(0, align_score);
}

TEST(LocalAlign, S1SameAsS2) {
	Sequence s1 = {"s1", "atcgc"};
	Sequence s2 = {"s2", "atcgc"};
	LocalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	int expected_score = dflt_params.match * s1.bps.size();
	EXPECT_EQ(expected_score, align_score);
}

TEST(LocalAlign, SingleInsert) {
	Sequence s1 = {"s1", "abcdefghi"};
	Sequence s2 = {"z + s1", "zabcdefghi"};
	LocalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(9 * dflt_params.match, align_score);
}

TEST(LocalAlign, MismatchSandwich) {
	Sequence s1 = {"s1", "abbbbbbbba"};
	Sequence s2 = {"s2", "acccccccca"};
	LocalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(dflt_params.match, align_score);
}

TEST(LocalAlign, GapSandwich) {
	Sequence s1 = {"s1", "aa"};
	Sequence s2 = {"s2", "abbbbbbba"};
	LocalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(dflt_params.match, align_score);
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
	LocalAligner aligner(s1_bps, s2_bps, std::pair<Sequence, Sequence>(s1, s2), params);
	int align_score = aligner.Align(PRINT_ALIGNMENT);
	EXPECT_EQ(55, align_score);
}

TEST(LocalAlign, Case0) {
	Sequence s1 = {"s1", "aaaaaaaaaaaaaaaaaabcbcbcbcbcbcaaaabcbcbcbcbcbcbcaaaaaaaaaaaaaaaa"};
	Sequence s2 = {"s2", "ddddbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbcbceeeeee"};
	LocalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
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
