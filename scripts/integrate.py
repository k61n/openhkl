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

file = Path('/home/zamaan/projects/datasets/120522b_after_integrate_pxsum336.nsx')
experiment = '120522b'
diffractometer = 'BioDiff2500'
data_name = 'soak_9_d2_I_scanI_9483.raw'
space_group = "P 21 21 21"
found_peaks_name = 'findint'
predicted_peaks_name = 'predict1'

expt = nsx.Experiment(experiment, diffractometer)
expt.loadFromFile(str(file))
data = expt.getData(data_name)
found_peaks = expt.getPeakCollection(found_peaks_name)
predicted_peaks = expt.getPeakCollection(predicted_peaks_name)

print('Generating shapes...')
expt.addEmptyShapeModel("shapes")
shapes = expt.getShapeModel("shapes")
shapes.build(found_peaks, data)
shape_params = shapes.parameters()

print('Assigning peak shapes...')
shape_params.neighbour_range_pixels = 500
shape_params.neighbour_range_frames = 10
shape_params.min_neighbours = 10
shape_params.interpolation = nsx.PeakInterpolation_InverseDistance
shapes.setPredictedShapes(predicted_peaks)

print('Integrating predicted peaks...')
# Integration parameters
integration_params = expt.integrator().parameters()
integration_params.integrator_type = nsx.IntegratorType_Profile3D
integrator = expt.integrator()
integrator.integratePeaks(data, predicted_peaks, integration_params, shapes)
print(f'{integrator.numberOfValidPeaks()} / {integrator.numberOfPeaks()} peaks integrated')

print('Merging predicted peaks...')

# Merge parameters
merger = expt.peakMerger()
params = merger.parameters()
merger.reset()
params.d_min = 1.5
merger.addPeakCollection(predicted_peaks)
merger.setSpaceGroup(nsx.SpaceGroup(space_group))
merger.mergePeaks()
merger.computeQuality()
print(merger.summary())

print("Integration complete")
