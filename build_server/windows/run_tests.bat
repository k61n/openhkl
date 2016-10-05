rem set BUILD_DIR=%cd%

rem set PATH=%BOOST_LIBRARYDIR%;%PATH%
rem set PATH=%FFTW_DIR%;%PATH%
rem set PATH=C:\Users\ci\Libraries\libtiff\lib;%PATH%
rem set PATH=C:\Program Files\HDF_Group\HDF5\1.8.17\bin;%PATH%
rem set PATH=%BUILD_DIR%\nsxlib\Release;%PATH%

ctest -V

