#include "suffix-tree/suffix-tree.h"
cc_test(
	name = "suffix-tree-test",
	srcs = ["suffix-tree-test.cc"],
	copts = ["-Iexternal/gtest/include"],
	deps = [
		"@gtest//:main",
		"//suffix-tree:suffix-tree-lib",
	],
	timeout="short",
)
