# Getting Started

## Install Bazel
https://docs.bazel.build/versions/master/install.html

## Build the suffix Tree builder program
From the root directory, run
```
bazel build suffix-tree
```

## (optionally) Run the tests
```
bazel test test:suffix-tree-test --test_output=all
``` 

## Run the suffix tree builder on your own files
```
$ ./<test executable> <input file containing sequence s> <input alphabet file>
```

```
./bazel-bin/suffix-tree/suffix-tree data/s1.fas data/English_alphabet.txt
```

# Outline
Simple unit tests are implemented using the Google Test framework in `test/`. If you build with bazel, the framework
should download automatically when you run `bazel test test:aligner-test --test_output=all`.



# TODO
