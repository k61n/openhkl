#!/bin/bash

cd ${CI_PROJECT_DIR}

mkdir ./build
cd build

cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_GSL=ON -DBUILD_QTAPPS=ON -DCMAKE_INSTALL_PREFIX=./ ..
cmake --build . --config Release
cmake --build . --config Release --target install

set NSX_ROOT_DIR=${CI_PROJECT_DIR}/share/nsxtool
