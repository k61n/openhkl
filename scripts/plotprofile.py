#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestTrypsin.py
##! @brief     Test full workflow for trypsin
##!
##! @homepage  https://openhkl.org
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import sys
import numpy as np
from matplotlib import pyplot as plt

lib_dir = "/home/zamaan/projects/openhkl/build/swig/"  # Path to pyohkl.py
sys.path.append(lib_dir)

import pyohkl as ohkl

filename = "Trypsin.ohkl"

expt = ohkl.Experiment('trypsin', 'BioDiff')
expt.loadFromFile(filename)

data = expt.getData('run a')
found_peaks = expt.getPeakCollection('found')
predicted_peaks = expt.getPeakCollection('predicted')
cell = expt.getUnitCell("indexed")

print("Experiment loaded")

# Build the ShapeModel (can't be saved to .ohkl file yet)
peak_end = 5.5
bkg_begin = 1.3
bkg_end = 2.3
region_type = ohkl.RegionType_FixedEllipsoid

found_peaks.computeSigmas()
shape_params = ohkl.ShapeModelParameters()
shape_params.sigma_d = found_peaks.sigmaD()
shape_params.sigma_m = found_peaks.sigmaM()
shape_params.d_min = 1.5
shape_params.d_max = 50.0
shape_params.strength_min = 5.0
shape_params.nbins_x = 20
shape_params.nbins_y = 20
shape_params.nbins_z = 10
shape_params.n_subdiv = 5
shape_params.min_n_neighbours = 5
shape_params.interpolation = ohkl.PeakInterpolation_NoInterpolation
shape_params.fixed_peak_end = peak_end
shape_params.fixed_bkg_begin = bkg_begin
shape_params.fixed_bkg_end = bkg_end
shape_params.region_type = region_type

found_peaks.buildShapeModel(shape_params)
shape_model = found_peaks.shapeModel()
print(f'ShapeModel built, contains {shape_model.numberOfPeaks()} shapes')

h = 7
k = -14
l = 9
ref_peak_index = ohkl.MillerIndex(h, k, l)
peak = found_peaks.findPeakByIndex(ref_peak_index)
print(f'Peak: ({h}, {k}, {l}) I = {peak.sumIntensity().value()}')
print(f'      center: {peak.shape().center().transpose()}')
intensity = peak.sumIntensity().value()
region = ohkl.IntegrationRegion(peak, peak_end, bkg_begin, bkg_end, region_type)
region_data = region.getRegion();

px_radius = 250
fr_radius = 10

region_data.buildProfile(shape_model, peak, px_radius, fr_radius)
nframes = region_data.nFrames()

intensity_min = 0
intensity_max = 2000

print (f'Peak appears on {nframes} images')

# Plot the integration regions and profiles
fig = plt.figure(figsize=(nframes*3, 9))
plt.tight_layout()
for i in range(nframes):
    ax = fig.add_subplot(1, nframes, i+1)
    ax.imshow(region_data.frame(i), vmin=intensity_min, vmax=intensity_max, cmap="Blues")
    ax = fig.add_subplot(2, nframes, i+1)
    ax.imshow(region_data.profileData(i, intensity), vmin=intensity_min, vmax=intensity_max, cmap="Blues")

plt.show()
