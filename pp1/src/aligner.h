#ifndef ALIGNER_H
#define ALIGNER_H

#include <stdio.h>
#include <stdlib.h>

#include <limits.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define MIN_N_ARGS 3
#define MAX_N_ARGS 4
#define DFLT_CONFIG_FNAME "parameters.config"

enum ALIGN_SCOPE {GLOBAL, LOCAL};

typedef struct dp_cell {
	int S;
	int D;
	int I;
} DP_CELL;

typedef struct score_config {
	int match;
	int mismatch;
	int h; // start gap penalty
	int g; // continuing gap penalty
} SCORE_CONFIG;

void print_help();
void print_score_config(const SCORE_CONFIG & scores);
void print_size(size_t asize);

void format_bps(std::string & str);

std::pair<std::string, std::string> load_sequences(const char* fasta_fname);

SCORE_CONFIG load_config(const char *config_fname);


ALIGN_SCOPE parse_align_scope(const char *alignment_str);

int cost2sub(char c1, char c2, const SCORE_CONFIG & scores);

int max3(int &i0, int &i1, int &i2);
int max3(DP_CELL & a);

char int_len(int n);

int align_global(std::string & s1, std::string & s2, const SCORE_CONFIG & scores);

#endif // ALIGNER_H