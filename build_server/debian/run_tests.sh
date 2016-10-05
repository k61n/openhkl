#!/bin/bash

cd ${CI_PROJECT_DIR}/build

ctest -j2 -l2
