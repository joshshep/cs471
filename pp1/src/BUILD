cc_binary(
	name = "aligner",
	srcs = glob(["*.cc"]),
	deps = [":alignerlib"],
)

cc_library(
	name = "alignerlib",
	srcs = glob(["*.cc"]),
	hdrs = glob(["*.h"]),
	visibility = ["//visibility:public"],
)