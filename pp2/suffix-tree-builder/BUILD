cc_binary(
	name = "suffix-tree-builder",
	srcs = glob(["main.cc"]),
	deps = [":suffix-tree-builder-lib"],
	copts = ["-O3", "-march=native", "-Wall", "-Wextra", "-pedantic", "-std=c++14"],
)

cc_library(
	name = "suffix-tree-builder-lib",
	srcs = glob(["suffix-tree-builder.cc", "util.cc"]),
	hdrs = glob(["suffix-tree-builder.h"]),
	visibility = ["//visibility:public"],
	copts = ["-O3", "-march=native", "-Wall", "-Wextra", "-pedantic", "-std=c++14"],
)
