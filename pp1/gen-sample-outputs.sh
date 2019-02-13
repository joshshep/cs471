#! /bin/bash

odir="sample-output"

mkdir -p $odir

ofile="$odir/colorblind_aligned_global.txt"
echo "Writing '$ofile'..."
./bazel-bin/aligner/aligner data/Opsin1_colorblindness_gene.fasta 0 > $ofile

ofile="$odir/colorblind_aligned_local.txt"
echo "Writing '$ofile'..."
./bazel-bin/aligner/aligner data/Opsin1_colorblindness_gene.fasta 1 > $ofile

ofile="$odir/brca2_aligned_global.txt"
echo "Writing '$ofile'..."
./bazel-bin/aligner/aligner data/Human-Mouse-BRCA2-cds.fasta 0 > $ofile

ofile="$odir/brca2_aligned_local.txt"
echo "Writing '$ofile'..."
./bazel-bin/aligner/aligner data/Human-Mouse-BRCA2-cds.fasta 1 > $ofile