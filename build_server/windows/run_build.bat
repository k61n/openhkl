mkdir build
cd build
set HDF5_ROOT=C:\Program Files\HDF_Group\HDF5\1.8.17
set QTDIR=C:\Qt\Qt5.6.1\5.6\msvc2015_64
set PATH=%QTDIR%\bin;%PATH%
set BOOST_ROOT=c:\local\boost_1_61_0
set BOOST_LIBRARYDIR=c:\local\boost_1_61_0\lib64-msvc-14.0
set FFTW_DIR=C:\Users\ci\Libraries\fftw
set PATH=C:\Users\ci\Libraries\eigen;%PATH%
set PATH=C:\Users\ci\Libraries\libtiff;%PATH%
set PATH=C:\Program Files\LLVM;%PATH%
call "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat"
cmake .. -G"Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -- -j4 -l4
cmake --build . --config Release --target install

