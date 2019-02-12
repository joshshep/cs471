#include "aligner/aligner.h"

/////////////////////////////
// public methods
/////////////////////////////
int Aligner::Align(bool print_alignment)
{
    int align_score = RunDP();
    if (print_alignment)
    {
        Alignment alignment = RetraceDP();
        PrintAlignment(alignment);
        PrintAlignStats(alignment);
    }
    return align_score;
}


/////////////////////////////
// protected methods
/////////////////////////////

void Aligner::InitDP()
{
    const int n_cols = s1_.size() + 1;
	const int n_rows = s2_.size() + 1;
	//std::cout << "Allocating dp table of size = " ;
	//print_size(n_cols * n_rows * sizeof(DP_CELL));
	//std::cout << " ..." << std::endl;

	// allocate memory for dp table
	dp_ = new DP_Cell*[n_rows];
	for (int j=0; j < n_rows; j++) {
		dp_[j] = new DP_Cell[n_cols];
	}

    // initialize edge values
	dp_[0][0] = {0};
	for (int i=1; i<n_cols; i++) {
		// TODO we want a value that's low enough not to conflict with table 
		//      values but high enough to avoid INT_MIN
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

void Aligner::DelDP()
{
	const int n_rows = s2_.size() + 1;

    for (int i = 0; i < n_rows; i++) {
		delete[] dp_[i];
	}
	delete[] dp_;
}

void Aligner::PrintAlignStats(Alignment alignment)
{
    std::cout << "Some stats" << std::endl;
}

void Aligner::PrintAlignmentLine(Alignment alignment, 
						const int i_retrace_start, int & i_s1, int & i_s2,
                        const int bp_per_line) {
    const std::string & retrace = alignment.retrace_str;
	const int i_retrace_end = std::min((int)retrace.size(), i_retrace_start + bp_per_line);

    // get maximum width of an index integer
	const int index_col_width = std::max(int_len(s1_.size()), int_len(s2_.size()));

	// print s1
	printf("s1 %*d  ", index_col_width, i_s1+1);
	for (int i_retrace = i_retrace_start; i_retrace < i_retrace_end; i_retrace++) {
		char c = retrace[i_retrace];
		switch(c) {
		case MATCH:
		case MISMATCH:
		case INSERT:
			std::cout << s1_[i_s1];
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
	printf("   %*s  ", index_col_width, "");
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
	printf("s2 %*d  ", index_col_width, i_s2+1);
	for (int i_retrace = i_retrace_start; i_retrace < i_retrace_end; i_retrace++) {
		char c = retrace[i_retrace];
		switch(c) {
		case MATCH:
		case MISMATCH:
		case DELETE:
			std::cout << s2_[i_s2];
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

void Aligner::PrintAlignment(Alignment alignment, const int bp_per_line) {
	int i_s1 = 0;
	int i_s2 = 0;
    //const std::string & retrace = alignment.retrace_str;
	int num_lines = 1 + alignment.retrace_str.size() / bp_per_line;
	PrintAlignmentLine(alignment, 0, i_s1, i_s2, bp_per_line);
	for (int iline = 1; iline<num_lines; iline++) {
		std::cout << std::endl;
		int i_retrace_start = iline * bp_per_line;
    	PrintAlignmentLine(alignment, i_retrace_start, i_s1, i_s2, bp_per_line);
	}
}




int Aligner::Cost2Sub(char c1, char c2) {
	if (c1 == c2) {
		return scoring_.match;
	}
	return scoring_.mismatch;
}





//////////////////////////////////////////////
// TODO: move below to subclass
//////////////////////////////////////////////

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

Alignment Aligner::RetraceDP(){
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

int Aligner::RunDP() {
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