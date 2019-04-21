#include "aligner/global-aligner.h"

namespace aligner {

void GlobalAligner::InitDP() {
	const int n_cols = s1_.size() + 1;
	const int n_rows = s2_.size() + 1;

	// initialize edge values
	dp_[0][0] = {0, 0, 0};
	for (int i=1; i<n_cols; i++) {
		// TODO we want a value that's low enough not to conflict with table 
		//      values but high enough to avoid underflowing INT_MIN
		//      we currently make the (simplifying) assumption that the table 
		//      size and ScoreConfig will not reach this low value
		dp_[0][i].S = INT_MIN >> 2;
		dp_[0][i].D = INT_MIN >> 2;
		dp_[0][i].I = scoring_.h + i * scoring_.g;
	}
	for (int j=1; j<n_rows; j++) {
		dp_[j][0].S = INT_MIN >> 2;
		dp_[j][0].D = scoring_.h + j * scoring_.g;
		dp_[j][0].I = INT_MIN >> 2;
	}
}

Alignment GlobalAligner::RetraceDP() {
	const int n_cols = s1_.size() + 1;
	const int n_rows = s2_.size() + 1;
	int i = n_cols - 1;
	int j = n_rows - 1;
	std::string retraced = "";
	RetraceState retrace_state;

	retrace_state = GetRetraceState(dp_[j][i], s1_[i-1], s2_[j-1]);
	
	// determine the Alignment by retracing from (i,j) to (0,0)
	while (true) {
		// append to our retrace step onto our retrace string
		retraced += (char) retrace_state;

		// move based on our retrace step
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

		// dp boundary check
		if (i < 1 || j < 1) {
			break;
		}
		DP_Cell & cell = dp_[j][i];

		// determine the retrace state (the next retrace step)
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
			retrace_state = GetRetraceState({i_d, i_i, i_s}, s1_[i-1], s2_[j-1]);
			break;
		}
		case DELETE:
		{
			int d_d = cell.D + scoring_.g;
			int d_s = cell.S + scoring_.g + scoring_.h;
			int d_i = cell.I + scoring_.g + scoring_.h;
			retrace_state = GetRetraceState({d_d, d_i, d_s}, s1_[i-1], s2_[j-1]);
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
	Alignment alignment = {retraced, 0, 0};
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


	// initialize the edge values in the dp table
	InitDP();

	const int n_cols = s1_.size() + 1;
	const int n_rows = s2_.size() + 1;

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

} // namespace aligner