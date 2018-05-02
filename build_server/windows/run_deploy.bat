set PROJECT_DIR=%cd%

set BUILD_DIR=%PROJECT_DIR%\build

C:\Qt\Qt5.6.1\5.6\msvc2015_64\bin\windeployqt %BUILD_DIR%\bin\nsxqt.exe

copy %BUILD_DIR%\externals\c-blosc\blosc\blosc.dll %BUILD_DIR%\bin\.

copy C:\opt\local_x64\bin\fftw3.dll %BUILD_DIR%\bin\.

copy C:\opt\local_x64\bin\tiff.dll %BUILD_DIR%\bin\.

copy C:\opt\local_x64\bin\zlib.dll %BUILD_DIR%\bin\.
copy C:\opt\local_x64\bin\hdf5.dll %BUILD_DIR%\bin\.
copy C:\opt\local_x64\bin\hdf5_cpp.dll %BUILD_DIR%\bin\.

makensis /V4 /DVERSION=1.0.0 /DTARGET_DIR=%BUILD_DIR% /DARCH="win-amd64" %PROJECT_DIR%\build_server\windows\build_nsxtool_installer.nsi
