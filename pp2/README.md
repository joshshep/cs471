# Getting Started

## Install Bazel
https://docs.bazel.build/versions/master/install.html

## Build the suffix Tree builder program
From the root directory, run
```
bazel build suffix-tree
```

## Run the suffix tree builder on your own files
```
$ ./<test executable> <input file containing sequence s> <input alphabet file>
```

```
./bazel-bin/suffix-tree/suffix-tree data/s1.fas data/English_alphabet.txt
```
