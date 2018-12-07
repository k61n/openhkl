rem Remove the resources before running the test
rmdir /S /Q %APPDATA%\\nsxtool

set BUILD_DIR=%cd%\build

set NSX_ROOT_DIR=%BUILD_DIR%\share\nsxtool

set PATH=C:\opt\local_x64\bin;%PATH%

cd %BUILD_DIR%

rem Some tests file needs to be convert to DOS to work properly
TYPE %BUILD_DIR%\tests\crystallography.tsv | C:\Python27\python.exe -c "import sys; sys.stdout.write(sys.stdin.read())" > %BUILD_DIR%\tests\crystallography.tsv_dos

MOVE /Y %BUILD_DIR%\tests\crystallography.tsv_dos %BUILD_DIR%\tests\crystallography.tsv

ctest --output-on-failure -V

exit %errorlevel%

