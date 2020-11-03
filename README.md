# veeam_test
Command line program in C++ to generate the signature of a given file. The signature
is generated as follows: the source file is divided into equal (fixed) length blocks. When the source file size
is not divisible by block size, the last fragment can be shorter or complemented with zeroes to full block
size. A hash value is calculated for each block and then added to the output signature file.
