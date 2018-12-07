#!/bin/bash

cd ${CI_PROJECT_DIR}

rm -rf build
mkdir ./build
cd build

cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_GSL=ON -DBUILD_NSX_APPS=ON -DCMAKE_INSTALL_PREFIX=./ -DUSE_PYTHON3=OFF ..
cmake --build . --config Release
cmake --build . --config Release --target install

set NSX_ROOT_DIR=${CI_PROJECT_DIR}/share/nsxtool
