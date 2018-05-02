set PROJECT_DIR=%cd%

set BUILD_DIR=%PROJECT_DIR%\build

C:\Qt\5.6.1\msvc2017_64\bin\windeployqt %BUILD_DIR%\build\bin\nsxqt.exe

copy %BUILD_DIR%\externals\c-blosc\blosc\Release\blosc.dll %BUILD_DIR%\build\bin\.

copy C:\opt\bin\fftw3.dll %BUILD_DIR%\build\bin\.

copy C:\opt\bin\tiff.dll %BUILD_DIR%\build\bin\.

copy C:\opt\bin\zlib.dll %BUILD_DIR%\build\bin\.
copy C:\opt\bin\hdf5.dll %BUILD_DIR%\build\bin\.
copy C:\opt\bin\hdf5_cpp.dll %BUILD_DIR%\build\bin\.

makensis /V4 /DVERSION=1.0.0 /DTARGET_DIR=%BUILD_DIR% /DARCH="win-amd64" %PROJECT_DIR%\build_server\windows\build_nsxtool_installer.nsi
