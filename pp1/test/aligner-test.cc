#include "gtest/gtest.h"
#include "aligner/local-aligner.h"
#include "aligner/global-aligner.h"


#define PRINT_RETRACE false
const ScoreConfig dflt_params = {
	.match = 1,
	.mismatch = -2,
	.h = -5,
	.g = -1
};

//testing::internal::CaptureStdout();

TEST(GlobalAlign, EmptyS1EmptyS2) {
	Sequence s1 = {.name = "empty str 1", .bps = ""};
	Sequence s2 = {.name = "empty str 2", .bps = ""};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
	EXPECT_EQ(0, align_score);
}

TEST(GlobalAlign, S1SameAsS2) {
	Sequence s1 = {.name = "s1", .bps = "atcgc"};
	Sequence s2 = {.name = "s2", .bps = "atcgc"};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
	int expected_score = dflt_params.match * s1.bps.size();
	EXPECT_EQ(expected_score, align_score);
}

TEST(GlobalAlign, SingleInsert) {
	Sequence s1 = {.name = "s1", .bps = "abcdefghi"};
	Sequence s2 = {.name = "z + s1", .bps = "zabcdefghi"};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
	EXPECT_EQ(3, align_score);
}

TEST(GlobalAlign, MismatchSandwich) {
	Sequence s1 = {.name = "s1", .bps = "abbbbbbbba"};
	Sequence s2 = {.name = "s2", .bps = "acccccccca"};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
	EXPECT_EQ(-14, align_score);
}

TEST(GlobalAlign, GapSandwich) {
	Sequence s1 = {.name = "s1", .bps = "aa"};
	Sequence s2 = {.name = "s2", .bps = "abbbbbbba"};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
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
	Sequence s1 = {.name = "s1", .bps = s1_bps};
	Sequence s2 = {.name = "s2", .bps = s2_bps};
	GlobalAligner aligner(s1_bps, s2_bps, std::pair<Sequence, Sequence>(s1, s2), params);
	int align_score = aligner.Align();
	EXPECT_EQ(55, align_score);
}





TEST(LocalAlign, EmptyS1EmptyS2) {
	Sequence s1 = {.name = "empty str 1", .bps = ""};
	Sequence s2 = {.name = "empty str 2", .bps = ""};
	GlobalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
	EXPECT_EQ(0, align_score);
}

TEST(LocalAlign, S1SameAsS2) {
	Sequence s1 = {.name = "s1", .bps = "atcgc"};
	Sequence s2 = {.name = "s2", .bps = "atcgc"};
	LocalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
	int expected_score = dflt_params.match * s1.bps.size();
	EXPECT_EQ(expected_score, align_score);
}

TEST(LocalAlign, SingleInsert) {
	Sequence s1 = {.name = "s1", .bps = "abcdefghi"};
	Sequence s2 = {.name = "z + s1", .bps = "zabcdefghi"};
	LocalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
	EXPECT_EQ(9 * dflt_params.match, align_score);
}

TEST(LocalAlign, MismatchSandwich) {
	Sequence s1 = {.name = "s1", .bps = "abbbbbbbba"};
	Sequence s2 = {.name = "s2", .bps = "acccccccca"};
	LocalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
	EXPECT_EQ(dflt_params.match, align_score);
}

TEST(LocalAlign, GapSandwich) {
	Sequence s1 = {.name = "s1", .bps = "aa"};
	Sequence s2 = {.name = "s2", .bps = "abbbbbbba"};
	LocalAligner aligner(s1.bps, s2.bps, std::pair<Sequence, Sequence>(s1, s2), dflt_params);
	int align_score = aligner.Align();
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
	Sequence s1 = {.name = "s1", .bps = s1_bps};
	Sequence s2 = {.name = "s2", .bps = s2_bps};
	LocalAligner aligner(s1_bps, s2_bps, std::pair<Sequence, Sequence>(s1, s2), params);
	int align_score = aligner.Align();
	EXPECT_EQ(55, align_score);
}

/*

TEST(AlignGlobal, EmptyS1ShortS2) {
	std::string s1 = "";
	std::string s2 = "abcdefg";
	EXPECT_EQ(dflt_params.h + s2.size()*dflt_params.g, align_global(s1, s2, dflt_params, PRINT_RETRACE));
}





TEST(FormatBps, UppercaseBps) {
	std::string s = "ATC";
	format_bps(s);
	EXPECT_EQ(s, std::string("atc"));
}

TEST(Cost2Sub, Match) {
	EXPECT_EQ(dflt_params.match, cost2sub('a', 'a', dflt_params));
}

TEST(Cost2Sub, Mismatch) {
	EXPECT_EQ(dflt_params.mismatch, cost2sub('a', 't', dflt_params));
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

*/

/*
// we can't open a file for some reasons
TEST(LoadSequences, cs471smallFile) {
	const char * fasta_fname = "../data/cs471_sample.fasta";
	std::vector<std::string> sequences = load_sequences(fasta_fname);
	EXPECT_EQ(2, sequences.size());
}
*/