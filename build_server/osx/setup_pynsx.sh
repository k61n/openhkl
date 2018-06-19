#!/bin/bash

NSX_APP_CONTENTS=/Users/ci/temp/nsxtool/build/qmake-build/nsxtool.app/Contents

# Setup the python to be used with pynsx

# Create the directory where Python packages will be copied
mkdir -p ${NSX_APP_CONTENTS}/lib

# Copy the python executable used for building pynsx
cp -r /System/Library/Frameworks/Python.framework/Versions/2.7/Python ${NSX_APP_CONTENTS}/Frameworks/libpython2.7.dylib
cp -r /System/Library/Frameworks/Python.framework/Versions/2.7/lib/* ${NSX_APP_CONTENTS}/lib 
cp -r /System/Library/Frameworks/Python.framework/Versions/2.7/Resources/Python.app/Contents/MacOS/Python ${NSX_APP_CONTENTS}/MacOS/python

chmod 777 ${NSX_APP_CONTENTS}/Frameworks/libpython2.7.dylib

mkdir ${NSX_APP_CONTENTS}/lib/python2.7/site-packages

# Add NumPy
cp -r /Library/Frameworks/Python.framework/Versions/2.7/lib/python2.7/site-packages/numpy ${NSX_APP_CONTENTS}/lib/python2.7/site-packages

# Copy pynsx.py and _pynsx.so in nsxtool app
cp /Users/ci/temp/nsxtool/build/swig/_pynsx.so ${NSX_APP_CONTENTS}/lib/python2.7/site-packages/.
cp /Users/ci/temp/nsxtool/build/swig/pynsx.py ${NSX_APP_CONTENTS}/lib/python2.7/site-packages/.

# Modify the library dynamic path to make python self-contained
install_name_tool -change /System/Library/Frameworks/Python.framework/Versions/2.7/Python @executable_path/../Frameworks/libpython2.7.dylib ${NSX_APP_CONTENTS}/MacOS/python
install_name_tool -id @executable_path/../Frameworks/libpython2.7.dylib ${NSX_APP_CONTENTS}/Frameworks/libpython2.7.dylib

# Modify the library dynamic path to make _pynsx binding self-contained
install_name_tool -change /System/Library/Frameworks/Python.framework/Versions/2.7/Python @executable_path/../Frameworks/libpython2.7.dylib ${NSX_APP_CONTENTS}/lib/python2.7/site-packages/_pynsx.so

# Modify the path to nsx library in _pynsx binding
install_name_tool -change /Users/ci/temp/nsxtool/build/nsxlib/libnsx.dylib @executable_path/../Frameworks/libnsx.dylib ${NSX_APP_CONTENTS}/lib/python2.7/site-packages/_pynsx.so

# Modify the path to libz for HDF5 and Tiff dylibs
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${NSX_APP_CONTENTS}/Frameworks/libhdf5.101.dylib
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${NSX_APP_CONTENTS}/Frameworks/libhdf5_cpp.101.dylib 
install_name_tool -change /usr/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib ${NSX_APP_CONTENTS}/Frameworks/libtiff.5.dylib 

