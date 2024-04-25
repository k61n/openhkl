#!/usr/bin/python
##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single-crystal diffraction
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
import matplotlib.pyplot as plt
from pathlib import Path

lib_dir = "@SWIG_INSTALL_PATH@"  # Path to pyohkl.py

sys.path.append(lib_dir)
import pyohkl as ohkl

file = Path('/home/zamaan/projects/openhkl/build/test/data/Trypsin-pxsum.ohkl')
experiment = 'Trypsin'
diffractometer = 'BioDiff'
data_name = 'Scan I'
space_group = "P 21 21 21"
found_peaks_name = 'found'
predicted_peaks_name = 'predicted'

expt = ohkl.Experiment(experiment, diffractometer)
expt.loadFromFile(str(file))
data = expt.getData(data_name)
found_peaks = expt.getPeakCollection(found_peaks_name)
predicted_peaks = expt.getPeakCollection(predicted_peaks_name)

expt.addEmptyShapeModel("shapes")
shapes = expt.getShapeModel("shapes")
shape_params = shapes.parameters()
shape_params.region_type = ohkl.RegionType_FixedEllipsoid
shape_params.fixed_peak_end = 5.5
shape_params.fixed_bkg_begin = 1.3
shape_params.fixed_bkg_end = 2.3
shape_params.use_max_width = False
shapes.build(found_peaks, data)

for frame_range in range(5, 30, 5):
    predicted_peaks.resetIntegration(ohkl.IntegratorType_Profile3D)

    shape_params.neighbour_range_pixels = 50
    shape_params.neighbour_range_frames = frame_range

    # Integration parameters
    integration_params = expt.integrator().parameters()
    integration_params.integrator_type = ohkl.IntegratorType_Profile3D
    integration_params.region_type = ohkl.RegionType_FixedEllipsoid
    integration_params.fixed_peak_end = 5.5
    integration_params.fixed_bkg_begin = 1.3
    integration_params.fixed_bkg_end = 2.3
    integration_params.discard_saturated = True
    integration_params.max_counts = 50000
    integration_params.use_max_strength = True
    integration_params.max_strength = 1.0
    integration_params.use_max_d = True
    integration_params.max_d = 2.56

    integrator = expt.integrator()
    integrator.integratePeaks(data, predicted_peaks, integration_params, shapes)

    # Merge parameters
    merger = expt.peakMerger()
    params = merger.parameters()
    merger.reset()
    params.d_min = 1.5
    params.d_max = 50.0
    params.n_shells = 10
    params.friedel = True
    merger.addPeakCollection(predicted_peaks)
    merger.setSpaceGroup(ohkl.SpaceGroup(space_group))
    merger.mergePeaks()
    merger.computeQuality()
    print("neighbour range (frame) = " + str(frame_range))
    print(merger.summary())

    d = merger.getFigureOfMerit(ohkl.FigureOfMerit_d, ohkl.IntegratorType_Profile3D)
    rpim = merger.getFigureOfMerit(ohkl.FigureOfMerit_Rpim, ohkl.IntegratorType_Profile3D)
    cchalf = merger.getFigureOfMerit(ohkl.FigureOfMerit_CChalf, ohkl.IntegratorType_Profile3D)

    plt.plot(d, cchalf, label=f'frame range = {frame_range}', linewidth=0.5)

plt.legend()
plt.savefig("trypsin_profile3d_framerange.pdf")
