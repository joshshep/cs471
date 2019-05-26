cc_binary(
	name = "read-map",
	srcs = glob(["main.cc"]),
	deps = [":read-map-lib"],
	copts = ["-O3", "-march=native", "-Wall", "-Wextra", "-pedantic", "-std=c++14"],
)

cc_library(
	name = "read-map-lib",
	srcs = ["read-map.cc", "read-map-worker.cc"],
	hdrs = ["read-map.h", "read-map-worker.h", "fixed-heap.h"],
	copts = ["-O3", "-march=native", "-Wall", "-Wextra", "-pedantic", "-std=c++14"],
	linkopts = ["-pthread"],
	deps = [
		"//suffix-tree:suffix-tree-lib",
		"//aligner:aligner-lib",
	],
	visibility = ["//visibility:public"],
)
