# Instructions for running Python scripts

## General comments

These python scripts access the C++ core via SWIG, which generates a shared
library called `_pynsx.o`, which is interfaced with `pynsx.py`. The scripts need
to know the location of this interface file, either by adding it to your
`$PYTHONPATH` environmental variable, or by modifying the following line at the
top of `scripts/nsx/workflow.py`:

```
sys.path.append("path/to/build/swig")
```

here, build is the cmake build directory.


In lieu of entering calculation parameters via a GUI, these scripts read them
from a file called `parameters`, which should be placed in the same directory as
the data files. It will look something like this:

```
a            55.04117
b            58.40894
c            66.77204
alpha        90.0
beta         90.0
gamma        90.0
spacegroup   P_21_21_21
n_vertices   2500
merging_d_min 1.5
angle_tol    0.05
min_strength 7.5
max_strength 10000000
radius       500
frames       5
```

The order of these key/value pairs is not important. The first sevn lines of
this example are the details of the unit cell (lattice parameters and space
group), which in this case are provided with the dataset. They are used by the
scripts to choose the correct solution after autoindexing (note that they do not
encode _all_ information about the unit cell, since the rotation matrix must be
calculated).

A full list of key/value pairs can be found in `scripts/nsx/parameters.py`; they
correspond to the parameters visible in the GUI.

## `scripts/workflow.py`

This script contains the entire NSXTool workflow in a largely automated form. It
will perform the following steps:

1. Load detector image data from raw(.tiff)/nexus/HDF5 files.
2. Autoindexing to find the unit cell and sample orientation:
  * Find peaks in the first few (default 10) frames of the dataset
  * Integrate the found peaks
  * Filter the peaks to remove any that are not sensible
  * Use a Fourier transform method to compute potential lattice vectors
  * Select the best solution by comparing with the given lattice parameters
3. Find all peaks in the data set
4. Integrate all peaks
5. Filter peaks to remove any that are:
   * Too close to the beam or to far from it
   * Too strong or too weak
   * Forbidden by symmetry considerations (N.B. not working yet)
6. Build a library of peak shapes from the strong peaks
7. Predict peaks from unit cell and symmetry
8. Fit strong peak shapes to weak and predicted peaks
9. Integrate predicted peaks
10. Merge found and predicted peaks,and compute quality metrics

```
usage: workflow.py [-h] --name NAME [--files FILES [FILES ...]] [--dataformat DATAFORMAT] --detector
                   DETECTOR [--loadnsx] [--predicted] [-p PARAMFILE]
                   [--max_autoindex_frames MAX_AUTOINDEX_FRAMES]
                   [--min_autoindex_frames MIN_AUTOINDEX_FRAMES] [--length_tol LENGTH_TOL]
                   [--angle_tol ANGLE_TOL] [--autoindex] [-v]

NSXTool workflow test script

optional arguments:
  -h, --help            show this help message and exit
  --name NAME           name of system
  --files FILES [FILES ...]
                        Data files
  --dataformat DATAFORMAT
                        Format of data files
  --detector DETECTOR   Type of detector
  --loadnsx             load <name>.nsx
  --predicted           Saved data in .nsx has completed prediction step
  -p PARAMFILE, --parameters PARAMFILE
                        File containing experiment paramters
  --max_autoindex_frames MAX_AUTOINDEX_FRAMES
                        Maximum number of frames to use for autoindexing
  --min_autoindex_frames MIN_AUTOINDEX_FRAMES
                        Minimum number of frames to use for autoindexing
  --length_tol LENGTH_TOL
                        length tolerance (a, b, c) for autoindexing
  --angle_tol ANGLE_TOL
                        angle tolerance (alpha, beta, gamma) for autoindexing
  --autoindex           Autoindex the data
  -v, --verbose         Print extra output
```

The script can be invoked as follows from the directory containing the data and
`parameters` files:
```
# for a single HDF5 file
/path/to/workflow.py --name trypsin --detector BioDiff2500 --files trypsin.hdf --dataformat hdf5 --autoindex
# for a collection of .tiff files
/path/to/workflow.py --name strep --detector BioDiff5000 --files *.tiff --dataformat raw --autoindex
```

The calculation is saved at two intermediate points, after steps 4 and 9 on the
list above. The calculation can then be restarted from the intermediate state
which contains all relevant data, including peak collections, unit cells and
data sets, using:

```
/path/to/workflow.py --name trypsin --detector BioDiff2500 --loadnsx
/path/to/workflow.py --name trypsin --detector BioDiff2500 --loadnsx --predicted
```

the former will load a calculation at step 4, and the latter at step 9.

All relevant information, including data quality metrics (CC, R-factors) is
written to the log file `nsx.log`.

### Important details

* The autoindexing step is very important because it computes the sample
  orientation matrix as well as the lattice vectors, and is also prone to
  failure without the right amount of data. It will start using the first few
  (default 10) frames of the dataset, and if this fails to index the system,
  then it will add another frame and try again, continuing unitil it reaches the
  maximum. These are set using the command line arguments
  `--min_autoindex_frames` and `--max_autoindex_frames`. This calculation is
  strongly dependent on the autoindexer parameters, visible in the
  `parameters.py` file. On the biological crystal systems I have successfully
  indexed, it seems that a key parameter is `n_vertices`, which sets the number
  of points on the unit spheres used to construct tiral vectors to use as
  lattice vectors (2500 seems to be the sweet spot, but this is obviously
  strongly system dependent).
  
* The command line arguments `--length-tol` and `--angle-tol` set the tolerances
  for choosing the correct unit cell from the autoindexer. In particular, if the
  crystal is known to be cubic, tetragonal or orthorhombic, it is advisable to
  set a tight angle tolerance, otherwise the list of compatible will be
  completely wrong.

## `scripts/autoindex/autoindex.py`

This script is useful for determining which autoindexer parameters (set via the
`parameters` file will work, since it is not generally obvious. Run the script
on the a set of .tiff raw data files using,

```
path/to/autoindextest.py --name strep --files *.tiff
```

This will run the autoindexer on a dataset generated by all `.tiff` files
provided as the argument. Note that the intermediate information will be saved
as `strep.nsx` (or whatever is provided by the `--name` flag) after the
integration step, and if this file is found for later calculations, it will
automatically be loaded.

The script will dump a list of solutions from the autoindexer to stdout:

  ```
  Quality         a         b         c     alpha      beta     gamma
  100.000  46.11586  93.09011 104.46404  90.00000  90.00000  90.00000
  100.000  45.66522  92.98052 210.51565  89.58908  89.61555  88.50348
  100.000  73.58283  73.85300 212.09144  85.99679  83.88316  78.19238
  100.000 328.53789  45.64939  92.96886  90.00000 106.03065  90.00000
  100.000  73.84290 130.27473 167.87320  89.23272  85.71576  76.68054
  100.000  73.82463 210.25625 212.40960  64.04887  86.03049  87.29217
  100.000  93.05285 172.02372 229.34880  97.24204 100.13018  90.85082
  100.000 172.87479  93.01095 253.38225  90.00000  93.86085  90.00000
   90.000 524.03768  93.06929 252.10529  90.00000  91.72697  90.00000
   40.000   8.94176  38.23128  73.37329  81.44158  88.95458  85.79302
  ```
