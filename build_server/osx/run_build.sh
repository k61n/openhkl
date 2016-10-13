#!/bin/bash

cd ${CI_PROJECT_DIR}

mkdir ./build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DCMAKE_INSTALL_PREFIX=. ..
make -j4 -l4
make install

