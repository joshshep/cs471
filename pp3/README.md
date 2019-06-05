# Read Map - Mapping read segments to a reference genome

## Getting started

### extract sample data
If you cloned this project from github, you will first need to decompress the data.zip file to `data/`
```
unzip data.zip
```

### Install bazel - Google's build tool
https://docs.bazel.build/versions/master/install.html

### Build the project:
```
bazel build read-map
```

or build (w/ asserts disabled)
```
bazel build read-map --compilation_mode=opt
```

### Run read map on a sample
```
./bazel-bin/read-map/read-map data/Peach_reference.fasta data/Peach_simulated_reads.fasta data/alpha.txt
```

## Outline
* Local alignment logic is contained in `aligner/`
* Suffix tree logic is contained in `suffix-tree/`
* The read mapping logic that makes use of the above projects is contained in `read-map/`.
* `docs/` contains the project description

## TODO
* figure out util funcs/share types
* mock test multithreading
* rm data unzip __MAXOSX
