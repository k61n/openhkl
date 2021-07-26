#!/bin/bash

declare -x QTDIR=/usr/local/Cellar/qt5/5.6.0

cd ${CI_PROJECT_DIR}

mkdir build

cd build

cmake -G"Ninja" -DCMAKE_BUILD_TYPE=Release -DNSX_PYTHON=ON -DUSE_PYTHON3=OFF -DBUILD_NSX_APPS=ON -DENABLE_CPACK=ON -DCMAKE_PREFIX_PATH="/Library/Frameworks/Python.framework/Versions/2.7;${QTDIR};/usr/local/Cellar/gsl/2.3" ..

cmake --build . --config Release --target package
