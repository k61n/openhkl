#*** Powershell Build Script ***
# This script is only intended for debugging purposes on MS-Windows platform.

# execute under windows:
# $ powershell -NoProfile -NonInteractive -ExecutionPolicy Bypass -f <build-script>
# build a particular MSBuild project:
# $ msbuild BornAgainBase.vcxproj [-target:BornAgainBase] -property:Configuration=Release

# set minimal path for build
$Env:PATH = "C:\Qt\msvc\bin;C:\Program Files\Python39\Scripts\;C:\Program Files\Python39\;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\Git\cmd;C:\opt\x64\include;C:\Program Files\CMake\bin;C:\opt\x64\boost_current\lib;C:\Program Files (x86)\NSIS;C:\Program Files\Ninja;C:\msys64\mingw64\bin;C:\Program Files\HDF_Group\HDF5\1.12.2\bin\;C:\Program Files\swigwin"

$OPT_DIR = "C:/opt/x64"
$QT_MSVC_DIR = "C:/Qt/msvc"
$QTCMake_DIR = "$QT_MSVC_DIR/lib/cmake"
$QTDIR = "$QT_MSVC_DIR"
$BOOST_DIR = "$OPT_DIR/boost_current"
$BOOST_INCLUDE_DIR = "$BOOST_DIR/include"
$HDF5_INCLUDE_DIR = "C:/Program Files/HDF_Group/HDF5/1.12.2/include"
$HDF5_LIB_DIR = "C:/Program Files/HDF_Group/HDF5/1.12.2/lib"
$INCLUDE_DIRS = "C:/opt/x64/include;$BOOST_INCLUDE_DIR;$HDF5_INCLUDE_DIR"
$LIB_DIRS = "C:/opt/x64/lib;$HDF5_LIB_DIR"
$SWIG_DIR = "C:/Program Files/swigwin"

mkdir build
cd build

cmake -G "Visual Studio 17 2022" -A x64 -T host=x64  -B . -S .. `
-DCMAKE_PREFIX_PATH="$OPT_DIR" -DQTDIR="$QT_MSVC_DIR" -DCMAKE_INCLUDE_PATH="$INCLUDE_DIRS" -DCMAKE_LIBRARY_PATH="$LIB_DIRS" -DCMAKE_C_COMPILER="cl.exe" -DCMAKE_CXX_COMPILER="cl.exe" `
-DBUILD_DOCUMENTATION=OFF -DBUILD_PDF_DOCUMENTATION=OFF `
-DTIFF_INCLUDE_DIR="C:/opt/x64/include/libtiff" `
-DOHKL_PYTHON=ON `

# cmake --build . --config Release
