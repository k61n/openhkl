#!/bin/bash

cd ${CI_PROJECT_DIR}

ctest -j2 -l2
