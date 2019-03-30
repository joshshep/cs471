cc_binary(
	name = "suffix-tree",
	srcs = glob(["main.cc"]),
	deps = [":suffix-tree-lib"],
	copts = ["-O3", "-march=native", "-Wall", "-Wextra", "-pedantic", "-std=c++14"],
)

cc_library(
	name = "suffix-tree-lib",
	srcs = glob(["suffix-tree.cc", "suffix-tree-node.cc", "util.cc"]),
	hdrs = glob(["suffix-tree.h", "suffix-tree-node.h"]),
	visibility = ["//visibility:public"],
	copts = ["-O3", "-march=native", "-Wall", "-Wextra", "-pedantic", "-std=c++14"],
)
