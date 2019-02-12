#include "aligner/global-aligner.h"

enum RETRACE_STATE get_retrace_state(const DP_Cell cell, char c_s1, char c_s2 ) {
	if (cell.S >= cell.D && cell.S >= cell.I) {
		// substitute
		if (c_s1 == c_s2) {
			return MATCH;
		}
		return MISMATCH;
	} else if (cell.D >= cell.S && cell.D >= cell.I) {
		// delete
		return DELETE;
		//j--;
	} else {
		// insert
		return INSERT;
		//i--;
	}
}

Alignment GlobalAligner::RetraceDP(){
	const int n_cols = s1_.size() + 1;
	const int n_rows = s2_.size() + 1;
	int i = n_cols - 1;
	int j = n_rows - 1;
	std::string retraced = "";
	enum RETRACE_STATE retrace_state;

	retrace_state = get_retrace_state(dp_[j][i], s1_[i-1], s2_[j-1]);
	
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
			break;
		}
		DP_Cell & cell = dp_[j][i];
		switch(retrace_state) {
		case MATCH:
		case MISMATCH:
		{
			retrace_state = get_retrace_state(cell, s1_[i-1], s2_[j-1]);
			break;
		}
		case INSERT:
		{
			int i_i = cell.I + scoring_.g;
			int i_s = cell.S + scoring_.g + scoring_.h;
			int i_d = cell.D + scoring_.g + scoring_.h;
			retrace_state = get_retrace_state({.D = i_d, .I = i_i, .S = i_s}, s1_[i-1], s2_[j-1]);
			break;
		}
		case DELETE:
		{
			int d_d = cell.D + scoring_.g;
			int d_s = cell.S + scoring_.g + scoring_.h;
			int d_i = cell.I + scoring_.g + scoring_.h;
			retrace_state = get_retrace_state({.D = d_d, .I = d_i, .S = d_s}, s1_[i-1], s2_[j-1]);
			break;
		}
		}
	}
	// if we're not at the origin, we'll need to add gaps to our retrace string
	for (; j > 0; j--) {
		retraced += (char) DELETE;
	}
	for (; i > 0; i--) {
		retraced += (char) INSERT;
	}
	//std::cout << "retraced: " << retraced << std::endl;
	std::reverse(retraced.begin(), retraced.end());
    Alignment alignment = {.retrace_str = retraced, .s1_start = 0, .s2_start = 0};
	return alignment;
}

int GlobalAligner::RunDP() {
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

	// main dp processing loop
	for (int j=1; j<n_rows; j++) {
		for (int i=1; i<n_cols; i++){
			// substitute
			dp_[j][i].S = max3(dp_[j-1][i-1]) + Cost2Sub(s1_[i-1], s2_[j-1]);

			// delete
			DP_Cell & cell_up = dp_[j-1][i];
			int d_d = cell_up.D + scoring_.g;
			int d_s = cell_up.S + scoring_.g + scoring_.h;
			int d_i = cell_up.I + scoring_.g + scoring_.h;
			dp_[j][i].D = max3(d_d, d_s, d_i);

			// insert
			DP_Cell & cell_left = dp_[j][i-1];
			int i_i = cell_left.I + scoring_.g;
			int i_s = cell_left.S + scoring_.g + scoring_.h;
			int i_d = cell_left.D + scoring_.g + scoring_.h;
			dp_[j][i].I = max3(i_i, i_s, i_d);
		}
	}
	int align_score = max3(dp_[n_rows-1][n_cols-1]);

	return align_score;
}