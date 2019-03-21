# Getting Started

## Install Bazel

https://docs.bazel.build/versions/master/install.html

## Build the PAT Tree builder program
From the root directory, run
```
bazel build suffix-tree-builder
```

## (optionally) Run the tests
```
bazel test test:suffix-tree-test --test_output=all
``` 

## Run alignment on your own files
TODO

# Outline
Simple unit tests are implemented using the Google Test framework in `test/`. If you build with bazel, the framework
should download automatically when you run `bazel test test:aligner-test --test_output=all`.



# TODO
