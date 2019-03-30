#! /bin/bash

# note: suffix-tree may need to be re-written to run this

for fname in data/*.{fas,fasta}; do
    # the alphabet doesn't matter
    ./bazel-bin/suffix-tree/suffix-tree $fname data/DNA_alphabet.txt
    echo 
done