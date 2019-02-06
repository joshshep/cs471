#include "src/aligner.h"

void print_help() {
	std::cout << "~$ ./<executable name> <input file containing both s1 and s2> <0: global, 1: local> <optional: path to parameters config file>" << std::endl;
	std::cout << "E.g., ~$ ./align gene.fasta 0 parameters.config" << std::endl;
}

// courtesy of this s/o post https://stackoverflow.com/a/1798170
std::string trim(const std::string& str,
                 const std::string& whitespace = " ")
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
		return ""; // no content

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}

void format_bps(std::string & str) {
	for (auto& c : str) {
		c = tolower(c);
	}
}


std::pair<std::string, std::string> load_sequences(const char* fasta_fname) {
	std::cout << "Opening fasta file '" << fasta_fname << "' ..." << std::endl;
	std::ifstream fasta_stream(fasta_fname);
	if(!fasta_stream) {
		std::cout << "Failed to open input file '" << fasta_fname << "'." << std::endl;
		exit(1);
	}

	std::string line, sequence;
	std::vector<std::string> sequences;
	bool iterating_through_seq = false;
	while (std::getline(fasta_stream, line)) {
		if (line.empty()) {
			if (iterating_through_seq) {
				sequences.push_back(sequence);
				sequence = "";
				iterating_through_seq = false;
			}
		} else {
			// non empty
			if (line[0] == '>') {
				// found header
				std::cout << "Reading sequence: '" << line << "' ..." << std::endl;
				iterating_through_seq = true;
			} else {
				line = trim(line);
				format_bps(line);
				sequence += line;
			}
		}
	}
	if (iterating_through_seq) {
		sequences.push_back(sequence);
	}

	if (sequences.size() != 2) {
		std::cout << "Improperly formatted input fasta file '" << fasta_fname << "'. Expected 2 sequences but found ";
		std::cout << sequences.size() << "." << std::endl;
		exit(1);
	}

	for(auto sequence : sequences) {
		//std::cout << "sequence: " << sequence << std::endl;
	}
	return std::pair<std::string, std::string>(sequences[0], sequences[1]);
}

ALIGN_SCOPE parse_align_scope(const char *alignment_str) {
	enum ALIGN_SCOPE align_scope;
	switch(alignment_str[0]) {
	case '0':
		align_scope = GLOBAL;
		break;
	case '1':
		align_scope = LOCAL;
		break;
	default:
		std::cout << "Failed to parse alignment scope: '" << alignment_str << "' (should be 0 or 1)" << std::endl;
		exit(1);
	}
	return align_scope;
}

void print_score_config(const SCORE_CONFIG & scores) {
	std::cout << "--Scoring configuration settings--" << std::endl;
	std::cout << "match: " << scores.match << std::endl;
	std::cout << "mismatch: " << scores.mismatch << std::endl;
	std::cout << "g: " << scores.g << std::endl;
	std::cout << "h: " << scores.h << std::endl;
}

SCORE_CONFIG load_config(const char *config_fname) {
	std::cout << "Loading configuration settings from file '" << config_fname << "' ..." << std::endl;
	std::ifstream config_stream(config_fname);
	if(!config_stream) {
		std::cout << "Failed to open config file '" << config_fname << "'." << std::endl;
		exit(1);
	}

	std::string line;
	SCORE_CONFIG scores;
	int provided = 0;
	while (std::getline(config_stream, line)) {
		// TODO use hashmap
		if (line.rfind("match ", 0) == 0) {
			provided |= 0b0001;
			int i = line.rfind(' ') + 1;
			scores.match = stoi(line.substr(i));
		} else if(line.rfind("mismatch ", 0) == 0) {
			provided |= 0b0010;
			int i = line.rfind(' ') + 1;
			scores.mismatch = stoi(line.substr(i));
		} else if(line.rfind("g ", 0) == 0) {
			provided |= 0b0100;
			int i = line.rfind(' ') + 1;
			scores.g = stoi(line.substr(i));
		} else if(line.rfind("h ", 0) == 0) {
			provided |= 0b1000;
			int i = line.rfind(' ') + 1;
			scores.h = stoi(line.substr(i));
		}
	}
	if (provided != 0b1111) {
		std::cout << "Warning: the parameter file '" << config_fname << "' did not provide values for each parameter (default value 0)" << std::endl;
	}
	print_score_config(scores);
	return scores;
}

void print_size(size_t asize) {
	if ((asize >> 30) > 0) {
		printf("%.2lf GiB", (double) asize / (1<<30));
	} else if ((asize >> 20) > 0) {
		printf("%.2lf MiB", (double) asize / (1<<20));
	} else if ((asize >> 10) > 0) {
		printf("%.2lf KiB", (double) asize / (1<<10));
	} else {
		printf("%d B", (int)asize);
	}
}

int cost2sub(char c1, char c2, const SCORE_CONFIG & scores) {
	if (c1 == c2) {
		return scores.match;
	}
	return scores.mismatch;
}

int max3(int &i0, int &i1, int &i2) {
	return std::max(i0, std::max(i1, i2));
}

int max3(DP_CELL & a) {
	return max3(a.D, a.I, a.S);
}

void print_dp_cell(DP_CELL & a) {
	printf("(I: %d, D: %d, S: %d)", a.I, a.D, a.S);
}

char uint_len(unsigned int i) {
	if(i >= 10) {
		return 1 + uint_len(i/10);
	}
	return 1;
}

char int_len(int i) {
	if (i < 0) {
		return 1 + uint_len(-i);
	}
	return uint_len(i);
}

void print_dp_table(DP_CELL** dp, std::string & s1, std::string & s2) {
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
		char bp = ' ';
		if (j > 0) {
			bp = s2[j-1];
		}
		printf("%5d %c |", j, bp);
		for (int i=0; i<n_cols; i++) {
			printf(" S: %*d |", col_widths[i], dp[j][i].S);
		}
		printf("\n");
		printf("        |");
		for (int i=0; i<n_cols; i++) {
			printf(" D: %*d |", col_widths[i], dp[j][i].D);
		}
		printf("\n");
		printf("        ");
		for (int i=0; i<n_cols; i++) {
			for (int idash=0; idash<6+col_widths[i]; idash++) {
				putchar('-');
			}
		}
		printf("-\n");
	}

	delete[] col_widths;
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
		dp[0][i].S = INT_MIN >> 1; // TODO low-ish?
		dp[0][i].D = INT_MIN >> 1;
		dp[0][i].I = scores.h + i * scores.g;
		/*
		printf("(i: %d, j: %d): ", i, 0);
		print_dp_cell(dp[0][i]);
		std::cout << std::endl;
		*/
	}
	for (int j=1; j<n_rows; j++) {
		dp[j][0].S = INT_MIN >> 1;
		dp[j][0].D = scores.h + j * scores.g;
		dp[j][0].I = INT_MIN >> 1;
		/*
		printf("(i: %d, j: %d): ", 0, j);
		print_dp_cell(dp[j][0]);
		std::cout << std::endl;
		*/
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

			/*
			printf("(i: %d, j: %d): ", i, j);
			print_dp_cell(dp[j][i]);
			std::cout << std::endl;
			*/
		}
	}
	//std::cout << "s1: " << s1 << std::endl;
	//std::cout << "s2: " << s2 << std::endl;
	//print_dp_table(dp, s1, s2);

	int align_score = max3(dp[n_rows-1][n_cols-1]);
	
	for (int i =0; i < n_rows; i++) {
		delete[] dp[i];
	}
	delete[] dp;

	return align_score;
}
