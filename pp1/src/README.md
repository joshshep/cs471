# Basepair alignment

TODO

# Getting Started

## Install Bazel

https://docs.bazel.build/versions/master/install.html

## Run the tests
```
bazel test //test:aligner-test --test_output=all
```

## Supply your own files
From the root directory, run a command of the form 
```
./<executable name> <input file containing both s1 and s2> <0: global, 1: local> <optional: path to parameters config file>
```
For example, 
```
./bazel-bin/src/aligner data/Opsin1_colorblindness_gene.fasta 0 parameters.config
```
