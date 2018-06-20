#!/bin/bash

cd ${CI_PROJECT_DIR}

mkdir build
cd build

cmake -G"Ninja" -DCMAKE_BUILD_TYPE=Release -DNSX_PYTHON=OFF -DBUILD_NSX_APPS=ON -DCMAKE_INSTALL_PREFIX=. -DCMAKE_PREFIX_PATH="/usr/local/Cellar/qt5/5.6.0;/usr/local/Cellar/gsl/2.3"  ..

cmake --build . --config Release

cmake --build . --config Release --target install

declare -x CPLUS_INCLUDE_PATH=/usr/local/include:/usr/local/opt/eigen/include/eigen3:/usr/local/opt/boost/include:${CI_PROJECT_DIR}/build/include/NSXTool:${CI_PROJECT_DIR}/apps/nsxqt

declare -x LIBRARY_PATH=${CI_PROJECT_DIR}/build/nsxlib:/usr/local/opt/boost/lib/:/usr/local/opt/fftw/lib/

mkdir qmake-build
cd qmake-build

/usr/local/opt/qt5/bin/qmake ${CI_PROJECT_DIR}/build/apps/NSXQt/NSXQt.pro CONFIG+=release
make -j4

# Prepare nsxtool.app for being a dmg
/usr/local/opt/qt5/bin/macdeployqt nsxtool.app/

# Add pynsx and its corresponding python to the dmg
chmod 755 ${CI_PROJECT_DIR}/build/fix_apple_bundle.sh
${CI_PROJECT_DIR}/build/fix_apple_bundle.sh

# Remove unnecessary files
rm -f *.cpp
rm -f *.h
rm -f *.o
rm Makefile

${CI_PROJECT_DIR}/build_server/osx/tools/create-dmg/create-dmg \
--background "${CI_PROJECT_DIR}/build_server/osx/resources/background.jpg" \
--volicon "${CI_PROJECT_DIR}/build_server/osx/resources/nsxtool.icns" \
--volname nsxtool \
--window-pos 200 120 \
--window-size 800 400 \
--icon nsxtool.app 200 190 \
--hide-extension nsxtool.app \
--app-drop-link 600 185 nsxtool.dmg .
