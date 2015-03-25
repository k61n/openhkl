# CMake generated Testfile for 
# Source directory: /users/pellegrini/Downloads/c-blosc-master/bench
# Build directory: /users/pellegrini/Downloads/c-blosc-master/buid/bench
# 
# This file includes the relevent testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(test_blosclz_1 "bench" "blosclz" "single" "1")
ADD_TEST(test_lz4_1 "bench" "lz4" "single" "1")
ADD_TEST(test_lz4hc_1 "bench" "lz4hc" "single" "1")
ADD_TEST(test_snappy_1 "bench" "snappy" "single" "1")
ADD_TEST(test_zlib_1 "bench" "zlib" "single" "1")
ADD_TEST(test_blosclz_n "bench" "blosclz" "single")
ADD_TEST(test_lz4_n "bench" "lz4" "single")
ADD_TEST(test_lz4hc_n "bench" "lz4hc" "single")
ADD_TEST(test_snappy_n "bench" "snappy" "single")
ADD_TEST(test_zlib_n "bench" "zlib" "single")
