#!/usr/bin/env python3
# coding: utf-8

import sys
sys.path.append("/home/zamaan/codes/nsxtool/nsxtool/build/swig")
import argparse
import pynsx as nsx
import math
import numpy as np
# import matplotlib.pyplot as plt
from scipy.optimize import least_squares
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSX Tool script for streptavidin+biotin')
parser.add_argument('--file', '-f', type=str, nargs='+', dest='files',
                    help='.tiff raw data files')
args = parser.parse_args()
filenames = args.files
number_of_files = len(filenames)

peak_collection_name = "strep"

################################################################################
## load configuration and raw data
################################################################################

expt = nsx.Experiment('streptavidin', 'BioDiff5000')
biodiff = expt.diffractometer()

data_params = nsx.RawDataReaderParameters()
data_params.wavelength = 2.67
data_params.delta_omega = 0.32
data_params.row_major = True
data_params.swap_endian = True
data_params.bpp = 2

reader = nsx.RawDataReader(filenames[0], biodiff)
reader.setParameters(data_params)

for filename in filenames[1:]:
    reader.addFrame(filename)

reader.end()
data = nsx.DataSet(reader)
expt.addData(data)

detector = data.reader().diffractometer().detector()
print("data loaded\n")

################################################################################
## find the peaks and their position on the map
################################################################################

print("Finding peaks...")

# get the peakfinder
finder = expt.peakFinder()
convolver = nsx.AnnularConvolver()
finder.setConvolver(convolver)

# set seach parameters
finder.setMinSize(30)
finder.setMaxSize(10000)
finder.setPeakScale(1.0)
finder.setThreshold(80.0)

# find the peaks
finder.find([data])
print("...peak finding complete\n")

##############################################################################
## Integrate the peaks
##############################################################################

print("Integrating...")
# Integration parameters
integrator_type = "Pixel sum integrator"
peak_area = 3.0
background_lower = 3.0
background_upper = 6.0

# Set parameters and do the integration
integrator = expt.getIntegrator(integrator_type)
integrator.setPeakEnd(peak_area)
integrator.setBkgBegin(background_lower)
integrator.setBkgEnd(background_upper)

expt.integrateFoundPeaks(integrator_type)

# the peak collection
peak_collection = finder.getPeakCollection()

print("...integration complete\n")

##############################################################################
## Filter the peaks
##############################################################################

print("Filtering...")
# Filter parameters
min_strength = 1.0
max_strength = 1.0e8
min_d_range = 1.5
max_d_range = 50.0

# set parameters and do the filtering
filter = expt.peakFilter()
filter.setDRange(min_d_range, max_d_range)
filter.setStrength(min_strength, max_strength)

filter.resetFiltering(peak_collection)
filter.filter(peak_collection)

# Peak counts
npeaks = peak_collection.numberOfPeaks()
ncaught = peak_collection.numberCaughtByFilter()
print(str(npeaks) + " peaks")
print(str(ncaught) + " peaks caught by filter")

print("...filtering complete\n")

##############################################################################
## Finding the unit cell
##############################################################################
print("Autoindexing...")

# Autoindexer parameters
max_dim = 200.0
n_solutions = 10
n_vertices = 1000
n_subdiv = 30
indexing_tol = 0.2
min_vol = 100.0

autoindexer_params = nsx.IndexerParameters()
autoindexer_params.maxdim = max_dim;
autoindexer_params.nSolutions = n_solutions;
autoindexer_params.nVertices = n_vertices;
autoindexer_params.subdiv = n_subdiv;
autoindexer_params.indexingTolerance = indexing_tol;
autoindexer_params.minUnitCellVolume = min_vol

auto_indexer = expt.autoIndexer()
auto_indexer.setParameters(autoindexer_params)
auto_indexer.autoIndex(peak_collection.getPeakList())
auto_indexer.rankSolutions()
auto_indexer.printSolutions()
print("...autoindexing complete\n")
