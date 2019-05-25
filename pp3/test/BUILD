cc_test(
	name = "read-map-test",
	srcs = glob(["*-test.cc"]),
	copts = ["-Iexternal/gtest/include"],
	deps = [
		"@gtest//:main",
		"//read-map:read-map-lib",
	],
	timeout="short",
	linkopts = ["-pthread"],
)
