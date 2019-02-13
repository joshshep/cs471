cc_binary(
	name = "aligner",
	srcs = glob(["main.cc"]),
	deps = [":alignerlib"],
	copts = ["-O3", "-march=native", "-Wall", "-Wextra", "-pedantic", "-std=c++14"],
)

cc_library(
	name = "alignerlib",
	srcs = glob(["aligner.cc", "global-aligner.cc", "local-aligner.cc", "util.cc"]),
	hdrs = glob(["aligner.h", "global-aligner.h", "local-aligner.h"]),
	visibility = ["//visibility:public"],
	copts = ["-O3", "-march=native", "-Wall", "-Wextra", "-pedantic", "-std=c++14"],
)
