#! /bin/bash

# note: suffix-tree may need to be re-written to run this

odir="generated-bwt"

mkdir -p $odir

ofile="$odir/Tomato_BWT.txt"
echo "Writing '$ofile' ..."
./bazel-bin/suffix-tree/suffix-tree data/Slyco.fas data/DNA_alphabet.txt > $ofile

ofile="$odir/Yeast_BWT.txt"
echo "Writing '$ofile' ..."
./bazel-bin/suffix-tree/suffix-tree data/chr12.fas data/DNA_alphabet.txt > $ofile

echo "done"