cc_binary(
	name = "aligner",
	srcs = glob(["main.cc"]),
	deps = [":alignerlib"],
)

cc_library(
	name = "alignerlib",
	srcs = glob(["aligner.cc"]),
	hdrs = glob(["aligner.h"]),
	visibility = ["//visibility:public"],
)
