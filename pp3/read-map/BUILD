cc_binary(
	name = "read-map",
	srcs = glob(["main.cc"]),
	deps = [":read-map-lib"],
	copts = ["-Wall", "-Wextra", "-pedantic", "-std=c++14"],
)

cc_library(
	name = "read-map-lib",
	srcs = ["read-map.cc", "read-map-worker.cc", "util.cc"],
	hdrs = ["read-map.h", "read-map-worker.h", "fixed-heap.h", "util.h"],
	copts = ["-Wall", "-Wextra", "-pedantic", "-std=c++14"],
	linkopts = ["-pthread"],
	deps = [
		"//suffix-tree:suffix-tree-lib",
		"//aligner:aligner-lib",
	],
	visibility = ["//visibility:public"],
)
