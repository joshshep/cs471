cc_test(
	name = "aligner-test",
	srcs = ["aligner-test.cc"],
	copts = ["-Iexternal/gtest/include"],
	deps = [
		"@gtest//:main",
		"//aligner:alignerlib",
	],
	timeout="short",
)
