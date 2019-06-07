#ifndef READ_MAP_UTIL_H_
#define READ_MAP_UTIL_H_

#include <assert.h>
#include <libgen.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>

#include "suffix-tree/suffix-tree.h"

namespace read_map {

void PrintHelp();

// modified from this s/o post https://stackoverflow.com/a/1798170
std::string Trim(const std::string& str,
                 const std::string& delimiter = " ");

void Format_bps(std::string & str);

std::vector<suffix_tree::Sequence> LoadSequencesVector(const char* fasta_fname);

void StripExt(char *s);

std::string GetOutputFilename(const char * reads_fname);

} // namespace read_map

#endif
