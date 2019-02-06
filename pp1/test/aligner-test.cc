#include "gtest/gtest.h"
#include "aligner/aligner.h"

const SCORE_CONFIG dflt_params = {
	.match = 1,
	.mismatch = -2,
	.h = -5,
	.g = -1
};

TEST(AlignGlobal, EmptyS1EmptyS2) {
	std::string s1 = "";
	std::string s2 = "";
	EXPECT_EQ(0, align_global(s1, s2, dflt_params));
}

TEST(AlignGlobal, EmptyS1ShortS2) {
	std::string s1 = "";
	std::string s2 = "abcdefg";
	EXPECT_EQ(dflt_params.h + s2.size()*dflt_params.g, align_global(s1, s2, dflt_params));
}

TEST(AlignGlobal, S1SameAsS2) {
	std::string s1 = "atcgc";
	std::string s2 = "atcgc";
	EXPECT_EQ(dflt_params.match * s1.size(), align_global(s1, s2, dflt_params));
}

TEST(AlignGlobal, SingleInsert) {
	std::string s1 = "abcdefghi";
	std::string s2 = "zabcdefghi";
	EXPECT_EQ(3, align_global(s1, s2, dflt_params));
}

TEST(AlignGlobal, cs471sample) {
	std::string s1 = "ACATGCTACACGTATCCGATACCCCGTAACCGATAACGATACACAGACCTCGTACGCTTG"
	                 "CTACAACGTACTCTATAACCGAGAACGATTGACATGCCTCGTACACATGCTACACGTACT"
	                 "CCGAT";
	std::string s2 = "ACATGCGACACTACTCCGATACCCCGTAACCGATAACGATACAGAGACCTCGTACGCTTG"
	                 "CTAATAACCGAGAACGATTGACATTCCTCGTACAGCTACACGTACT"
	                 "CCGAT";
	const SCORE_CONFIG params = {
		.match = 1,
		.mismatch = -2,
		.h = -5,
		.g = -2
	};
	int alignment_score = align_global(s1, s2, params);
	EXPECT_EQ(55, alignment_score);
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

/*
// we can't open a file for some reasons
TEST(LoadSequences, cs471smallFile) {
	const char * fasta_fname = "../data/cs471_sample.fasta";
	std::vector<std::string> sequences = load_sequences(fasta_fname);
	EXPECT_EQ(2, sequences.size());
}
*/