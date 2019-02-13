# Getting Started

## Install Bazel

https://docs.bazel.build/versions/master/install.html

## Build the alignment program
From the root directory, run
```
bazel build aligner
```

## (optionally) Run the tests
```
bazel test test:aligner-test --test_output=all
``` 

## Run alignment on your own files
From the root directory, run a command of the form 
```
./<executable name> <input file containing both s1 and s2> <0: global, 1: local> <optional: path to parameters config file>
```
For example, 
```
./bazel-bin/aligner/aligner data/cs471_sample.fasta 0
```

# Outline
We have 3 main classes: Aligner, GlobalAligner, and LocalAligner. Aligner defines a relatively general abstract class 
for performing alignments. GlobalAligner and LocalAligner inherit Aligner to implement the Needleman-Wunsch algorithm
and Smith-Waterman algorithm respectively. These are defined (in addition to some utility functions) in `aligner/`.

Simple unit tests are implemented using the Google Test framework in `test/`. If you build with bazel, the framework
should download automatically when you run `bazel test [...]`.



# TODO
* bit-packing bp chars
* linear space alignment
* linear space retrace