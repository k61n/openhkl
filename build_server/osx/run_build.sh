#!/bin/bash

cd ${CI_PROJECT_DIR}

mkdir ./build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DCMAKE_PREFIX_PATH=/usr/local/Cellar/qt5/5.6.0 -DCMAKE_INSTALL_PREFIX=. -DBUILD_WITH_OPENMP=OFF ..
make -j4 -l4
make install

