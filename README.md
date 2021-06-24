# NSXTool

## Provisional installation instructions (Development version)

### Linux

1. Clone this repository:
```
git clone https://jugit.fz-juelich.de/mlz/nsxtool.git
```

2. Install global dependencies. These are generally installed at a system level via a package management system on Linux or MacOS.
    - Blosc
    - C++ compiler (e.g. g++ or clang)
    - cmake
    - Eigen3
    - FFTW3
    - GSL
	- HDF5 - assuming this is installed on the system level, it will be located automatically
    - Python3 interpreter
    - QCustomPlot2
    - QHull (esp. libqhullcpp)
    - QT5
    - YAMLCpp

3. Install local dependencies. Assuming here that they are being installed in `~/Software`, which contains the directories `include` and `lib` with various headers and libraries installed.
    - XSection (https://jugit.fz-juelich.de/mlz/xsection)
    - QCR (https://jugit.fz-juelich.de/mlz/libqcr)

```
git clone https://jugit.fz-juelich.de/mlz/repo.git # Assuming this is now in directory "repo"
cd repo
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=~/Software ..
make
make install
```

4. Run `cmake`, and `make`:
```
cd /path/to/nsxtool
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=~/Software ..
make
```

5. Note that at present, NSXTool has no `make install` target, so the code must be run from the `build` directory. The GUI executable is `build/main/NSXTool`. The Python scripts access a SWIG-generated shared library interfaced via `build/swig/pynsx.py`, which should be added to the `PYTHONPATH` environmental variable:
```
PYTHONPATH=$PYTHONPATH:/path/to/nsxtool/build/swig
```

### MacOS (Catalina)

This is largely the same as for Linux, but I use Homebrew [brew.sh](brew.sh), but beware that if you have multiple package managers on MacOS, they can interfere and break something critical. Homebrew will also install any higher order dependencies you need.

```
brew install python3 cmake qt5 eigen hdf5 swig c-blosc fftw cpp-gsl
pip3 install numpy scipy
```

Homebrew does not add QT5 paths to /usr/local/bin, so you will need to use the follwing argument when running `cmake`:

```
cmake -DCMAKE_PREFIX_PATH=/usr/local/opt/qt ..
```

`QCustomPlot` is not available via Homebrew, so you will have to [download](www.qcustomplot.com) the both the source and shared library compilation packages. The installation instructions are provided, but to summarise them:
1. Download the [source](https://www.qcustomplot.com/release/2.0.1/QCustomPlot-source.tar.gz)
2. Download the [shared library configuration scripts](https://www.qcusmplot.com/release/2.0.1/QCustomPlot-sharedlib.tar.gz)
3. Unpack the source to some directory, e.g. `~/Software/qcustomplot`
4. Unpack the shared library scripts to the root directory containing the source, `~/Software/qcustomplot`
5. Go to `~/Software/qcustomplot/sharedlibs/sharedlib-compilation`
6. Run `qmake` followed by `make`. This will generate two libraries and some symlinks of the form `libqcustomplot.dylib` and `libqcustomplotd.dylib`, the latter of which contains debugging flags, and you can ignore.

Things are complicated by the fact that there is no `make install` target for these libraries, which will normally set the correct `rpath` metadata in the shared library file. In this instance, the `rpath` variable is not set, as evidenced by the `otool` output:

```
$ otool -D libqcustomplot.dylib
libqcustomplot.dylib:
libqcustomplot.2.dylib
```

The correct path can be set using `install_name_tool`, and the correct path confirmed via `otool`:
```
$ install_name_tool -id "@rpath/libqcustomplot.dylib" libqcustomplot.dylib

$ otool -D libqcustomplot.dylib
libqcustomplot.dylib:
@rpath/libqcustomplot.2.dylib
```

Without this fix, `cmake` will use the `libqcustomplot.dylib` metadata to set the path to the library, and not be able to find it. Although the code will compile, it will crash at runtime with an error message resembling,

```
dyld: Library not loaded: libqcustomplot.dylib
  Referenced from: ./main/NSXTool
  Reason: image not found
```

The NSXTool executable can be checked using `otool` from the NSXTool build directory:

```
$ otool -L main/NSXTool

...
@rpath/libxsection.0.dylib (compatibility version 0.0.0, current version 0.1.0)
@rpath/libQCR.0.dylib (cmopatibility version 0.0.0, current version 0.2.0)
@rpath/libqcustomplot.dylib (compatibility version 2.0.0, current version 2.0.1)
...
```

Without the fix, the QCustomPlot line would read `libqcustomplot.dylib (compatibility version 2.0.0, current version 2.0.1)`.

The rest of the compilation prodedure is step 3 of the Linux instructions onwards. The only difference is that `cmake` must be pointed towards `libqcustomplot.dylib` and `qcustomplot.h`. This can either be achieved by simply copying the shared libraries to the directory referenced by the `CMAKE_INSTALL_PREFIX` variable, in my case `~/Software`, before running `install_name_tool` as above.
