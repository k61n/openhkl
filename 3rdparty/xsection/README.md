# xsection

This is the home page of **xsection**,
a self-contained C++ library for neutron and x-ray scattering cross sections.

## License

[GNU Public License version 3 or higher](http://opensource.org/licenses/TODO).

## Contact

For bug reports or suggestions, contact the maintainer: j.wuttke@fz-juelich.de.

# Installation

## From source

Get the latest source archive (tgz) from
[Repository > Tags](https://jugit.fz-juelich.de/mlz/xsection/tags).

Build&install are based on CMake. Out-of-source build is enforced. After unpacking the source, go to the source directory and do:

```
mkdir build
cd build
cmake ..
ctest
make
make install
```
