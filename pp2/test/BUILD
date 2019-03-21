#include "suffix-tree-builder/suffix-tree-builder.h"
cc_test(
	name = "suffix-tree-test",
	srcs = ["suffix-tree-test.cc"],
	copts = ["-Iexternal/gtest/include"],
	deps = [
		"@gtest//:main",
		"//suffix-tree-builder:suffix-tree-builder-lib",
	],
	timeout="short",
)
