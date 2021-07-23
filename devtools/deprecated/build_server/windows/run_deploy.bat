set PROJECT_DIR=%cd%

set BUILD_DIR=%PROJECT_DIR%\build

%QTDIR%\bin\windeployqt %BUILD_DIR%\bin\nsxqt.exe

copy %BUILD_DIR%\externals\c-blosc\blosc\blosc.dll %BUILD_DIR%\bin\.

copy C:\opt\local_x64\bin\fftw3.dll %BUILD_DIR%\bin\.

copy C:\opt\local_x64\bin\tiff.dll %BUILD_DIR%\bin\.

copy C:\opt\local_x64\bin\zlib.dll %BUILD_DIR%\bin\.
copy C:\opt\local_x64\bin\hdf5.dll %BUILD_DIR%\bin\.
copy C:\opt\local_x64\bin\hdf5_cpp.dll %BUILD_DIR%\bin\.

cd %PROJECT_DIR%\build_server\windows

makensis /V4 /DVERSION=1.0.0 /DTARGET_DIR=%BUILD_DIR% /DARCH="win-amd64" .\build_nsxtool_installer.nsi

exit %errorlevel%
