cc_test(
	name = "read-map-test",
	srcs = glob(["*-test.cc"]),
	copts = ["-Iexternal/gtest/include", "-std=c++14"],
	deps = [
		"@gtest//:main",
		"//read-map:read-map-lib",
	],
	timeout="short",
)

cc_test(
	name = "suffix-tree-test",
	srcs = glob(["suffix-tree-test.cc"]),
	copts = ["-Iexternal/gtest/include", "-std=c++14"],
	deps = [
		"@gtest//:main",
		"//read-map:read-map-lib",
	],
	timeout="short",
)
