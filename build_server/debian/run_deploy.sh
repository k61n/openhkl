#!/bin/bash

cd ${CI_PROJECT_DIR}
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_GSL=ON -DBUILD_QTAPPS=ON .
make package

