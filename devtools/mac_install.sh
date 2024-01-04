#!/bin/bash

macdeployqt main/OpenHKL.app
codesign --deep --force --verify --verbose --sign "Zamaan Raza" main/OpenHKL.app

hdiutil create -size 300m -fs HFS+ -volname "OpenHKL" -attach ./OpenHKL.dmg
mv OpenHKL.app /Volumes/OpenHKL/
rm -rf /Volumes/OpenHKL/.fseventsd
ln -s /Applications/ /Volumes/OpenHKL/Applications
hdiutil detach /Volumes/OpenHKL
hdiutil convert OpenHKL.dmg -format UDZO -o OpenHKL-v.0.1-arm.dmg
