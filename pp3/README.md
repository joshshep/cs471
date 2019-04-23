# Read Map

## Getting started

you will first need to decompress the data.zip file

Build the project:
```
bazel build read-map
```

build (w/ asserts disabled)
```
bazel build read-map --compilation_mode=opt
```


Run read map on an example
```
./bazel-bin/read-map/read-map data/Peach_reference.fasta data/Peach_simulated_reads.fasta
```

## Running tests
```
bazel test test:read-map-test --test_output=all
```