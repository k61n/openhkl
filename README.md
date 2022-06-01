# OpenHKL

## Provisional installation instructions (Development version)

### Linux

1. Clone this repository:
```
git clone https://jugit.fz-juelich.de/mlz/openhkl.git
```

2. Install global dependencies. These are generally installed at a system level via a package management system on Linux or MacOS.
    - Blosc
    - C++ compiler (e.g. g++ or clang)
    - cmake
    - Eigen3
    - FFTW3
    - GSL
	- HDF5
    - Python3
    - QHull
    - QT5/6
    - YAMLCpp

3. Run `cmake`, and `make`:
```
cd /path/to/nsxtool
mkdir build
cd build
cmake ..
make
```

4. Note that at present, OpenHKL has no `make install` target, so the code must be run from the `build` directory. The GUI executable is `build/main/NSXTool`. The Python scripts access a SWIG-generated shared library interfaced via `build/swig/pynsx.py`, which should be added to the `PYTHONPATH` environmental variable:
```
PYTHONPATH=$PYTHONPATH:/path/to/nsxtool/build/swig
```

### MacOS (Monterey)

This is largely the same as for Linux, but I use Homebrew [brew.sh](brew.sh), but beware that if you have multiple package managers on MacOS, they can interfere and break something critical. Homebrew will also install any higher order dependencies you need.

```
brew install python3 cmake qt5 eigen hdf5 swig c-blosc fftw cpp-gsl
pip install numpy scipy
```

Homebrew does not add QT paths to /usr/local/bin, so you will need to use the follwing argument when running `cmake`:

```
cmake -DCMAKE_PREFIX_PATH=/opt/homebrew/opt/qt ..
```

The rest of the compilation prodedure is step 3 of the Linux instructions onwards.
