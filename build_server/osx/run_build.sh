#!/bin/bash

cd ${CI_PROJECT_DIR}

mkdir ./build

cd build

cmake -G"Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_QTAPPS=ON -DCMAKE_PREFIX_PATH="/usr/local/Cellar/qt5/5.6.0;/usr/local/Cellar/gsl/2.3" -DCMAKE_INSTALL_PREFIX=. ..

cmake --build . --config Release

cmake --build . --config Release --target install

