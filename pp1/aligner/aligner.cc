#include "aligner/aligner.h"

int Aligner::Align(bool print_alignment) {
	int align_score = RunDP();
	if (print_alignment) {
		Alignment alignment = RetraceDP();
		PrintAlignment(alignment);
		std::cout << std::endl;
		PrintAlignStats(alignment);
		std::cout << std::endl;
		std::cout << "Alignment score: " << align_score << std::endl;
		//PrintDP_Table(dp_, s1_, s2_);
	}
	return align_score;
}


/////////////////////////////
// protected methods
/////////////////////////////

void Aligner::InitDP() {
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
		//      values but high enough to avoid underflowing INT_MIN
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

void Aligner::DelDP() {
	const int n_rows = s2_.size() + 1;

	for (int i = 0; i < n_rows; i++) {
		delete[] dp_[i];
	}
	delete[] dp_;
}

RetraceState Aligner::GetRetraceState(const DP_Cell cell, char c_s1, char c_s2 ) {
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

void Aligner::PrintAlignStats(Alignment alignment) {
	int num_matches = 0;
	int num_mismatches = 0;
	int num_gaps = 0;
	int num_gap_openings = 0;
	char prev_c = 0;
	for (auto c : alignment.retrace) {
		switch (c) {
		case MATCH:
			num_matches++;
			break;
		case MISMATCH:
			num_mismatches++;
			break;
		case INSERT:
		case DELETE:
			num_gaps++;
			if (c != prev_c) {
				num_gap_openings++;
			}
			break;
		}
		prev_c = c;
	}
	std::cout << "Occurrences:" << std::endl;
	std::cout << "  matches:      " << num_matches << std::endl;
	std::cout << "  mismatches:   " << num_mismatches << std::endl;
	std::cout << "  gaps:         " << num_gaps << std::endl;
	std::cout << "  gap openings: " << num_gap_openings << std::endl;

	std::cout << std::endl;

	int perc_identities = int(0.5 + 100.0 *  num_matches / alignment.retrace.size());
	int perc_gaps = int(0.5 + 100.0 *  num_gaps / alignment.retrace.size());
	printf("Identities: %d/%lu (%d%%)\n", num_matches, alignment.retrace.size(), perc_identities);
	printf("Gaps:       %d/%lu (%d%%)\n", num_gaps, alignment.retrace.size(), perc_gaps);
}

void Aligner::PrintAlignmentLine(const std::string & retrace, 
	                             const int i_retrace_start, int & i_s1, int & i_s2,
	                             const int bp_per_line) {
	const int i_retrace_end = std::min((int)retrace.size(), i_retrace_start + bp_per_line);

    // get maximum width of an index integer
	const int index_col_width = std::max(intLen(s1_.size()), intLen(s2_.size()));

	// print s1
	printf("s1  %*d  ", index_col_width, i_s1+1);
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
	printf("    %*s  ", index_col_width, "");
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
	printf("s2  %*d  ", index_col_width, i_s2+1);
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
	int i_s1 = alignment.s1_start;
	int i_s2 = alignment.s2_start;
	int num_lines = 1 + (alignment.retrace.size() - 1) / bp_per_line;
	PrintAlignmentLine(alignment.retrace, 0, i_s1, i_s2, bp_per_line);
	for (int iline = 1; iline<num_lines; iline++) {
		std::cout << std::endl;
		int i_retrace_start = iline * bp_per_line;
		PrintAlignmentLine(alignment.retrace, i_retrace_start, i_s1, i_s2, bp_per_line);
	}
}

int Aligner::Cost2Sub(char c1, char c2) {
	if (c1 == c2) {
		return scoring_.match;
	}
	return scoring_.mismatch;
}