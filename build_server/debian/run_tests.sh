#!/bin/bash

# Remove the resources before running the test
rm -rf ~/.nsxtool

cd ${CI_PROJECT_DIR}/build
export CTEST_OUTPUT_ON_FAILURE=1
ctest -j2 -l2
