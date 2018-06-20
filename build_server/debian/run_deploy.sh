#!/bin/bash

cd ${CI_PROJECT_DIR}
cd build

cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_GSL=ON -DBUILD_DEBIAN=ON -DBUILD_NSX_APPS=ON .
cmake --build . --config Release --target package
