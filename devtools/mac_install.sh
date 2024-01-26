#!/bin/bash

gcc_lib_dir="/opt/homebrew/Cellar/gcc/13.2.0/lib/gcc/current"

macdeployqt main/OpenHKL.app
cp ${gcc_lib_dir}/libgcc_s.1.1.dylib main/OpenHKL.app/Contents/Frameworks
cp ${gcc_lib_dir}/libquadmath.0.dylib main/OpenHKL.app/Contents/Frameworks
codesign --deep --force --verify --verbose --sign "Zamaan Raza" main/OpenHKL.app

hdiutil create -size 300m -fs HFS+ -volname "OpenHKL" -attach ./OpenHKL.dmg
mv main/OpenHKL.app /Volumes/OpenHKL/
rm -rf /Volumes/OpenHKL/.fseventsd
ln -s /Applications/ /Volumes/OpenHKL/Applications
hdiutil detach /Volumes/OpenHKL
hdiutil convert OpenHKL.dmg -format UDZO -o OpenHKL-v.0.1-arm.dmg
