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
