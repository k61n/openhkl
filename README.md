# NSXTool

## Provisional installation instructions (Development version, Linux/MacOS)

1. Clone this repository:
```
git clone https://jugit.fz-juelich.de/mlz/nsxtool.git
```

2. Install global dependencies. These are generally installed at a system level via a package management system on Linux or MacOS.
    1. cmake
    2. C++ compiler (generally g++ or clang for Linux)
    3. Python3 interpreter
	4. HDF5 - assuming this is installed on the system level, it will be located automatically
    5. QT5
    6. QCustomPlot2
    7. Eigen3
    8. YAMLCpp
    9. Boost
    10. GSL
    11. FFTW3
    12. Blosc

3. Install local dependencies. Assuming here that they are being installed in `~/Software`, which contains the directories `include` and `lib` with various headers and libraries installed.
    1. XSection (https://jugit.fz-juelich.de/mlz/xsection)
    2. QCR (https://jugit.fz-juelich.de/mlz/libqcr)

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

5. The GUI executable is `build/main/NSXTool`. The Python scripts access a SWIG-generated shared library interfaced via `build/swig/pynsx.py`, which should be added to the `PYTHONPATH` environmental variable:
```
PYTHONPATH=$PYTHONPATH:/path/to/nsxtool/build/swig
```
