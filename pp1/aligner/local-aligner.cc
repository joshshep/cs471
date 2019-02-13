#include "aligner/local-aligner.h"

void LocalAligner::MaxCellInDP(int &i_max, int &j_max) {
	// TODO should we make n_cols a const member val?
	const int n_cols = s1_.size() + 1;
	const int n_rows = s2_.size() + 1;
	int max_val = 0;
	for (int j=0; j<n_rows; j++)
	{
		for (int i=0; i<n_cols; i++)
		{
			int cand_max = max3(dp_[j][i]);
			if (cand_max > max_val)
			{
				i_max = i;
				j_max = j;
				max_val = cand_max;
			}
		}
	}
}

Alignment LocalAligner::RetraceDP() {
	int i = 0;
	int j = 0;
	std::string retraced = "";
	enum RETRACE_STATE retrace_state;

	// get ending point for retrace
	MaxCellInDP(i, j);
	std::cout << "Max cell in dp (i,j): (" << i << "," << j << ")" << std::endl;

	retrace_state = GetRetraceState(dp_[j][i], s1_[i-1], s2_[j-1]);
	
	while (true) {
		// TODO fix this mess
		retraced += (char) retrace_state;

		switch(retrace_state) {
		case MATCH:
		case MISMATCH:
			i--; j--;
			break;
		case INSERT:
			i--;
			break;
		case DELETE:
			j--;
			break;
		}

		if (i < 1 || j < 1) {
			std::cout << "retrace DP loop: reached edge of dp table" << std::endl;
			break;
		}
		DP_Cell & cell = dp_[j][i];

		if (max3(cell) <= 0) {
			std::cout << "retrace DP loop: found zero value" << std::endl;
			break;
		}
		switch(retrace_state) {
		case MATCH:
		case MISMATCH:
		{
			retrace_state = GetRetraceState(cell, s1_[i-1], s2_[j-1]);
			break;
		}
		case INSERT:
		{
			int i_i = cell.I + scoring_.g;
			int i_s = cell.S + scoring_.g + scoring_.h;
			int i_d = cell.D + scoring_.g + scoring_.h;
			retrace_state = GetRetraceState({.D = i_d, .I = i_i, .S = i_s}, s1_[i-1], s2_[j-1]);
			break;
		}
		case DELETE:
		{
			int d_d = cell.D + scoring_.g;
			int d_s = cell.S + scoring_.g + scoring_.h;
			int d_i = cell.I + scoring_.g + scoring_.h;
			retrace_state = GetRetraceState({.D = d_d, .I = d_i, .S = d_s}, s1_[i-1], s2_[j-1]);
			break;
		}
		}
	}
	/*
	for (; j > 0; j--) {
		retraced += (char) DELETE;
	}
	for (; i > 0; i--) {
		retraced += (char) INSERT;
	}
	*/
	//std::cout << "retraced: " << retraced << std::endl;
	std::reverse(retraced.begin(), retraced.end());
    Alignment alignment = {.retrace = retraced, .s1_start = i, .s2_start = j};
	return alignment;
}

int LocalAligner::RunDP() {
	/*
	           s1
	   __________________ m
	   |
	   |
	s2 |
	   |
	   |
	   n
	*/
	const int n_cols = s1_.size() + 1;
	const int n_rows = s2_.size() + 1;
	//std::cout << "Allocating dp table of size = " ;
	//print_size(n_cols * n_rows * sizeof(DP_CELL));
	//std::cout << " ..." << std::endl;

	int align_score = 0;
	// main dp processing loop
	for (int j=1; j<n_rows; j++) {
		for (int i=1; i<n_cols; i++){
			// substitute
			dp_[j][i].S = std::max(0, max3(dp_[j-1][i-1])) + Cost2Sub(s1_[i-1], s2_[j-1]);

			// delete
			DP_Cell & cell_up = dp_[j-1][i];
			int d_d = cell_up.D + scoring_.g;
			int d_s = cell_up.S + scoring_.g + scoring_.h;
			int d_i = cell_up.I + scoring_.g + scoring_.h;
			dp_[j][i].D = max3(d_d, d_s, d_i);
			dp_[j][i].D = std::max(0, dp_[j][i].D);


			// insert
			DP_Cell & cell_left = dp_[j][i-1];
			int i_i = cell_left.I + scoring_.g;
			int i_s = cell_left.S + scoring_.g + scoring_.h;
			int i_d = cell_left.D + scoring_.g + scoring_.h;
			dp_[j][i].I = max3(i_i, i_s, i_d);
			dp_[j][i].I = std::max(0, dp_[j][i].I);

			// the alignment score is the max value in the dp table
			// TODO: we assume that scores for gaps are negative
			align_score = std::max(align_score, max3(dp_[j][i]));
		}
	}

	return align_score;
}