#!/usr/bin/python
##  ***********************************************************************************************
##
##  OpenHKL: data reduction for neutron single-crystal diffraction
##
##! @file      test/python/TestWorkFlow.py
##! @brief     Test ...
##!
##! @homepage  https://openhkl.org
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
diffractometer = 'BioDiff'
data_name = 'soak_9_d2_I_scanI_9483.raw'
space_group = "P 21 21 21"
found_peaks_name = 'findint'
predicted_peaks_name = 'predict1'

expt = nsx.Experiment(experiment, diffractometer)
expt.loadFromFile(str(file))
data = expt.getData(data_name)
found_peaks = expt.getPeakCollection(found_peaks_name)
predicted_peaks = expt.getPeakCollection(predicted_peaks_name)

expt.addEmptyShapeModel("shapes")
shapes = expt.getShapeModel("shapes")
shapes.build(found_peaks, data)
shape_params = shapes.parameters()

for f_range in range(4, 18, 2):
    name = "frange_" + str(f_range);
    print(name)
    shape_params.neighbour_range_pixels = 400
    shape_params.neighbour_range_frames = f_range
    shape_params.interpolation = nsx.PeakInterpolation_InverseDistance
    shapes.setPredictedShapes(predicted_peaks)

    # Integration parameters
    integration_params = expt.integrator().parameters()
    integration_params.integrator_type = nsx.IntegratorType_PixelSum
    integrator = expt.integrator()
    integrator.integratePeaks(data, predicted_peaks, integration_params, shapes)

    # Merge parameters
    merger = expt.peakMerger()
    params = merger.parameters()
    merger.reset()
    params.d_min = 1.5
    merger.addPeakCollection(predicted_peaks)
    merger.setSpaceGroup(nsx.SpaceGroup(space_group))
    merger.mergePeaks()
    merger.computeQuality()
    with open(name, "w") as outfile:
        outfile.write(merger.summary())
    print(merger.summary())
