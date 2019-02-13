#! /bin/bash

odir="sample-output"

mkdir -p $odir

echo "Writing colorblind global..."
./bazel-bin/aligner/aligner data/Opsin1_colorblindness_gene.fasta 0 > "$odir/colorblind_aligned_global.txt"

echo "Writing colorblind local..."
./bazel-bin/aligner/aligner data/Opsin1_colorblindness_gene.fasta 1 > "$odir/colorblind_aligned_local.txt"

echo "Writing brca2 global..."
./bazel-bin/aligner/aligner data/Human-Mouse-BRCA2-cds.fasta 0 > "$odir/brca2_aligned_global.txt"

echo "Writing brca2 local..."
./bazel-bin/aligner/aligner data/Human-Mouse-BRCA2-cds.fasta 1 > "$odir/brca2_aligned_local.txt"