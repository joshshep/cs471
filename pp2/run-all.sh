#! /bin/bash

# note: suffix-tree may need to be re-written to run this

logfile=runtime.log

# delete log
> $logfile

for fname in `ls -rS data/*.{fas,fasta}`; do
    # the alphabet doesn't matter
    /usr/bin/time -lp ./bazel-bin/suffix-tree/suffix-tree $fname data/DNA_alphabet.txt | tee -a $logfile
    echo | tee -a $logfile
done