cmake_minimum_required(VERSION 2.8)

add_library( bz2 blocksort.c   huffman.c  crctable.c randtable.c compress.c decompress.c bzlib.c )