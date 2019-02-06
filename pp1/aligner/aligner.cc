#include "aligner/aligner.h"




int cost2sub(char c1, char c2, const SCORE_CONFIG & scores) {
	if (c1 == c2) {
		return scores.match;
	}
	return scores.mismatch;
}

void print_dp_cell(DP_CELL & a) {
	printf("(I: %d, D: %d, S: %d)", a.I, a.D, a.S);
}

void print_dp_table(DP_CELL** dp, const std::string & s1, const std::string & s2) {
	int n_cols = s1.size() + 1;
	int n_rows = s2.size() + 1;
	char* col_widths = new char[n_cols];

	// get column widths
	for (int i=0; i<n_cols; i++) {
		col_widths[i] = 0;
		for (int j=0; j<n_rows; j++) {
			col_widths[i] = std::max(
				std::max(col_widths[i], int_len(dp[j][i].I)), 
				std::max(int_len(dp[j][i].S), int_len(dp[j][i].D))
			);
		}
		//printf("col_widths[%2d] = %d\n", i, col_widths[i]);
	}

	// print numbers on first row
	printf("     ");
	for (int i=0; i<n_cols; i++) {
		char bp = ' ';
		if (i > 0) {
			bp = s1[i-1];
		}
		printf("  %*d %c  ", col_widths[i], i, bp);
	}
	printf("\n");
	// print top line
	printf("        ");
	for (int i=0; i<n_cols; i++) {
		for (int idash=0; idash<6+col_widths[i]; idash++) {
			putchar('-');
		}
	}
	printf("-\n");

	for (int j=0; j<n_rows; j++) {
		printf("        |");
		for (int i=0; i<n_cols; i++) {
			printf(" I: %*d |", col_widths[i], dp[j][i].I);
		}
		printf("\n");
		char j_bp = ' ';
		if (j > 0) {
			j_bp = s2[j-1];
		}
		printf("%5d %c |", j, j_bp);
		for (int i=0; i<n_cols; i++) {
			printf(" S: %*d %c", col_widths[i], dp[j][i].S, j_bp);
		}
		printf("\n");
		printf("        |");
		for (int i=0; i<n_cols; i++) {
			printf(" D: %*d |", col_widths[i], dp[j][i].D);
		}
		printf("\n");
		printf("        ");
		for (int i=0; i<n_cols; i++) {
			int num_dashes = 6+col_widths[i];
			for (int idash=0; idash<num_dashes/2; idash++) {
				putchar('-');
			}
			if (i > 0) {
				putchar(s1[i-1]);
			} else {
				putchar('-');
			}
			for (int idash=num_dashes/2 + 1; idash<num_dashes; idash++) {
				putchar('-');
			}
		}
		printf("-\n");
	}

	delete[] col_widths;
}

enum RETRACE_STATE get_retrace_state(const DP_CELL cell, char c_s1, char c_s2 ) {
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

std::string gen_retrace_str(DP_CELL** dp, const std::string & s1, const std::string & s2, const SCORE_CONFIG scores){
	const int n_cols = s1.size() + 1;
	const int n_rows = s2.size() + 1;
	int i = n_cols - 1;
	int j = n_rows - 1;
	std::string retraced = "";
	enum RETRACE_STATE retrace_state;

	retrace_state = get_retrace_state(dp[j][i], s1[i-1], s2[j-1]);
	
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
		DP_CELL & cell = dp[j][i];
		switch(retrace_state) {
		case MATCH:
		case MISMATCH:
		{
			retrace_state = get_retrace_state(cell, s1[i-1], s2[j-1]);
			break;
		}
		case INSERT:
		{
			int i_i = cell.I + scores.g;
			int i_s = cell.S + scores.g + scores.h;
			int i_d = cell.D + scores.g + scores.h;
			retrace_state = get_retrace_state({.D = i_d, .I = i_i, .S = i_s}, s1[i-1], s2[j-1]);
			break;
		}
		case DELETE:
		{
			int d_d = cell.D + scores.g;
			int d_s = cell.S + scores.g + scores.h;
			int d_i = cell.I + scores.g + scores.h;
			retrace_state = get_retrace_state({.D = d_d, .I = d_i, .S = d_s}, s1[i-1], s2[j-1]);
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

	return retraced;
}

void print_retrace_line(const std::string & retrace, const int i_retrace_start,
                        const std::string & s1, int & i_s1,
                        const std::string & s2, int & i_s2,
                        const int bp_per_line) {

	const int i_retrace_end = std::min((int)retrace.size(), i_retrace_start + bp_per_line);

	const int index_col_width = std::max(int_len(s1.size()), int_len(s2.size()));
	// print s1
	printf("%*d  ", index_col_width, i_s1+1);
	for (int i_retrace = i_retrace_start; i_retrace < i_retrace_end; i_retrace++) {
		char c = retrace[i_retrace];
		switch(c) {
		case MATCH:
		case MISMATCH:
		case INSERT:
			std::cout << s1[i_s1];
			i_s1++;
			break;
		case DELETE:
			std::cout << '-';
			break;
		}
	}
	printf("  %*d", index_col_width, i_s1);
	std::cout << std::endl;

	// print matching
	printf("%*s  ", index_col_width, "");
	for (int i_retrace = i_retrace_start; i_retrace < i_retrace_end; i_retrace++) {
		char c = retrace[i_retrace];
		switch(c) {
		case MATCH:
			std::cout << c;
			break;
		case MISMATCH:
		case INSERT:
		case DELETE:
			std::cout << ' ';
			break;
		}
	}
	std::cout << std::endl;

	// print s2
	printf("%*d  ", index_col_width, i_s2+1);
	for (int i_retrace = i_retrace_start; i_retrace < i_retrace_end; i_retrace++) {
		char c = retrace[i_retrace];
		switch(c) {
		case MATCH:
		case MISMATCH:
		case DELETE:
			std::cout << s2[i_s2];
			i_s2++;
			break;
		case INSERT:
			std::cout << '-';
			break;
		}
	}
	printf("  %*d", index_col_width, i_s2);
	std::cout << std::endl;
}

void print_retrace_str(const std::string & retrace, const std::string & s1, const std::string & s2, const int bp_per_line = DFLT_BP_PER_LINE) {
	int i_s1 = 0;
	int i_s2 = 0;
	int num_lines = 1 + retrace.size() / bp_per_line;
	print_retrace_line(retrace, 0, s1, i_s1, s2, i_s2, bp_per_line);
	for (int iline = 1; iline<num_lines; iline++) {
		std::cout << std::endl;
		int i_retrace_start = iline * bp_per_line;
		print_retrace_line(retrace, i_retrace_start, s1, i_s1, s2, i_s2, bp_per_line);
	}
}

int align_global(std::string & s1, std::string & s2, const SCORE_CONFIG & scores) {
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
	const int n_cols = s1.size() + 1;
	const int n_rows = s2.size() + 1;
	//std::cout << "Allocating dp table of size = " ;
	//print_size(n_cols * n_rows * sizeof(DP_CELL));
	//std::cout << " ..." << std::endl;

	// allocate memory for dp table
	DP_CELL** dp = new DP_CELL*[n_rows];
	for (int j=0; j < n_rows; j++) {
		dp[j] = new DP_CELL[n_cols];
	}

	// initialize edge values
	dp[0][0] = {0};
	for (int i=1; i<n_cols; i++) {
		// TODO we want a value that's low enough not to conflict with table 
		//      values but high enough to avoid INT_MIN
		dp[0][i].S = INT_MIN >> 2;
		dp[0][i].D = INT_MIN >> 2;
		dp[0][i].I = scores.h + i * scores.g;
	}
	for (int j=1; j<n_rows; j++) {
		dp[j][0].S = INT_MIN >> 2;
		dp[j][0].D = scores.h + j * scores.g;
		dp[j][0].I = INT_MIN >> 2;
	}

	// main dp processing loop
	for (int j=1; j<n_rows; j++) {
		for (int i=1; i<n_cols; i++){
			// substitute
			dp[j][i].S = max3(dp[j-1][i-1]) + cost2sub(s1[i-1], s2[j-1], scores);

			// delete
			DP_CELL & cell_up = dp[j-1][i];
			int d_d = cell_up.D + scores.g;
			int d_s = cell_up.S + scores.g + scores.h;
			int d_i = cell_up.I + scores.g + scores.h;
			dp[j][i].D = max3(d_d, d_s, d_i);

			// insert
			DP_CELL & cell_left = dp[j][i-1];
			int i_i = cell_left.I + scores.g;
			int i_s = cell_left.S + scores.g + scores.h;
			int i_d = cell_left.D + scores.g + scores.h;
			dp[j][i].I = max3(i_i, i_s, i_d);
		}
	}
	//std::cout << "s1: " << s1 << std::endl;
	//std::cout << "s2: " << s2 << std::endl;
	//print_dp_table(dp, s1, s2);
	std::string retrace_str = gen_retrace_str(dp, s1, s2, scores);
	print_retrace_str(retrace_str, s1, s2);
	int align_score = max3(dp[n_rows-1][n_cols-1]);
	
	for (int i =0; i < n_rows; i++) {
		delete[] dp[i];
	}
	delete[] dp;

	return align_score;
}
