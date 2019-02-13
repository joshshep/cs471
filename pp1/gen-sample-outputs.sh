#! /bin/bash

echo "Writing colorblind global..."
./bazel-bin/aligner/aligner data/Opsin1_colorblindness_gene.fasta 0 > colorblind_aligned_global.txt

echo "Writing colorblind local..."
./bazel-bin/aligner/aligner data/Opsin1_colorblindness_gene.fasta 1 > colorblind_aligned_local.txt

echo "Writing brca2 global..."
./bazel-bin/aligner/aligner data/Human-Mouse-BRCA2-cds.fasta 0 > brca2_aligned_global.txt

echo "Writing brca2 local..."
./bazel-bin/aligner/aligner data/Human-Mouse-BRCA2-cds.fasta 1 > brca2_aligned_local.txt