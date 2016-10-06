#!/bin/bash

cd ${CI_PROJECT_DIR}/build

ctest -j4 -l4

