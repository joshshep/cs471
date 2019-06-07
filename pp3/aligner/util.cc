#include "aligner/aligner.h"

namespace aligner {

AlignmentScope ParseAlignScope(const char *alignment_str) {
	enum AlignmentScope align_scope;
	switch (alignment_str[0]) {
	case '0':
		std::cout << "Running global alignment..." << std::endl;
		align_scope = GLOBAL;
		break;
	case '1':
		std::cout << "Running local alignment..." << std::endl;
		align_scope = LOCAL;
		break;
	default:
		std::cout << "Failed to parse alignment scope: '" << alignment_str << "' (should be 0 or 1)" << std::endl;
		exit(1);
	}
	return align_scope;
}

void PrintScoreConfig(const ScoreConfig & scores) {
	std::cout << "-- Scoring configuration settings --" << std::endl;
	std::cout << "  match:    " << scores.match << std::endl;
	std::cout << "  mismatch: " << scores.mismatch << std::endl;
	std::cout << "  g:        " << scores.g << std::endl;
	std::cout << "  h:        " << scores.h << std::endl;
}

ScoreConfig LoadConfig(const char *config_fname) {
	std::cout << "Loading configuration settings from file '" << config_fname << "' ..." << std::endl;
	std::ifstream config_stream(config_fname);
	if (!config_stream) {
		std::cout << "Failed to open config file '" << config_fname << "'." << std::endl;
		exit(1);
	}

	std::string line;
	ScoreConfig scores;
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
	PrintScoreConfig(scores);
	return scores;
}

void PrintSize(size_t asize) {
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

char uintLen(unsigned int i) {
	if (i >= 10) {
		return 1 + uintLen(i/10);
	}
	return 1;
}

char intLen(int i) {
	if (i < 0) {
		return 1 + uintLen(-i);
	}
	return uintLen(i);
}

int max3(const DP_Cell & a) {
	return std::max({a.D, a.I, a.S});
}

void PrintDP_Table(DP_Cell** dp, const std::string & s1, const std::string & s2) {
	int n_cols = s1.size() + 1;
	int n_rows = s2.size() + 1;
	char* col_widths = new char[n_cols];

	// get column widths
	for (int i=0; i<n_cols; i++) {
		col_widths[i] = 0;
		for (int j=0; j<n_rows; j++) {
			col_widths[i] = std::max(
				std::max(col_widths[i], intLen(dp[j][i].I)), 
				std::max(intLen(dp[j][i].S), intLen(dp[j][i].D))
			);
		}
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

} // namespace aligner
