rmdir /s /q build
mkdir build
cd build

set BOOST_ROOT=C:\opt\local_x64
set FFTW_DIR=C:\opt\local_x64
set GSL_ROOT_DIR=C:\opt\local_x64
set HDF5_ROOT=C:\opt\local_x64

set PATH=C:\opt\local_x64\bin;%PATH%

call "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\vcvarsall.bat" amd64

cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=%QTDIR%\lib\cmake -DYAML_ROOT_DIR=C:\opt\local_x64 -DTIFF_ROOT=C:\opt\local_x64 -DZLIB_ROOT=C:\opt\local_x64 -DNSX_PYTHON=OFF -DCMAKE_INSTALL_PREFIX=.
cmake --build . --config Release
cmake --build . --config Release --target install

cd ..

exit %errorlevel%
