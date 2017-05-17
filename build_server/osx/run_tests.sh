#!/bin/bash

cd ${CI_PROJECT_DIR}/build

export OMP_THREAD_LIMIT=1
export CTEST_OUTPUT_ON_FAILURE=1

ninja test

