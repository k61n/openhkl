#!/usr/bin/python
##  ***********************************************************************************************
##
##  NSXTool: data reduction for neutron single-crystal diffraction
##
##! @file      test/python/TestWorkFlow.py
##! @brief     Test ...
##!
##! @homepage  ###HOMEPAGE###
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import sys
import numpy as np
from pathlib import Path

lib_dir = "@SWIG_INSTALL_PATH@"  # Path to pynsx.py

sys.path.append(lib_dir)
import pynsx as nsx

file = Path('/home/zamaan/projects/250422b_trypsin.nsx')
experiment = '250422b'
diffractometer = 'BioDiff2500'
data_name = 'soak_9_d2_I_scanI_9483.raw'
found_peaks_name = 'findint'
predicted_peaks_name = 'predict1'

if (file.is_file()):
    print('Found .nsx file ' + str(file))
else:
    print('Could not find file ' + str(file))

expt = nsx.Experiment(experiment, diffractometer)
expt.loadFromFile(str(file))
data = expt.getData(data_name)
found_peaks = expt.getPeakCollection(found_peaks_name)
predicted_peaks = expt.getPeakCollection(predicted_peaks_name)

def get_shapes(data, peaks, params):
    peaks.computeSigmas()
    params.sigma_d = peaks.sigmaD()
    params.sigma_m = peaks.sigmaM()
    peaks.buildShapeModel(data, params)
    print(f'{peaks.shapeCollection().numberOfPeaks()} shapes generated')
    return peaks.shapeCollection()

def integrate(integrator_type, data, peaks, shapes, params):
    integrator = expt.integrator()
    integrator.integratePeaks(integrator_type, data, peaks)
    print(f'{integrator.numberOfValidPeaks()} / {integrator.numberOfPeaks()} peaks integrated')


print('Generating shapes...')
params = nsx.ShapeModelParameters()
shapes = get_shapes(data, found_peaks, params)

print('Assigning peak shapes...')
params.neighbour_range_pixels = 500
params.neighbour_range_frames = 10
params.min_neighbours = 10
interpolation_type = nsx.PeakInterpolation_NoInterpolation
# found_peaks.shapeCollection().setPredictedShapes(predicted_peaks, interpolation_type)


print('Integrating predicted peaks...')
# Integration parameters
params = expt.integrator().parameters()
integrator_type = nsx.IntegratorType_Profile3D
integrate(integrator_type, data, predicted_peaks, shapes, params)

print('Merging predicted peaks...')

# Merge parameters
merger = expt.peakMerger()
params = merger.parameters()
merger.reset()
params.d_min = 1.5
params.frame_min = 1  # exclude first and last frames from statistics
params.frame_max = data.nFrames() - 1
merger.addPeakCollection(predicted_peaks)
merger.mergePeaks()
merger.computeQuality()

print("Integration complete")
