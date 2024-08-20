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

file = Path('@CMAKE_BINARY_DIR@/test/data/Trypsin.ohkl')
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

for px_range in np.linspace(25, 200, num=8):
    predicted_peaks.resetIntegration(ohkl.IntegratorType_Profile3D)

    # Shapes
    shape_params = expt.shapeParameters()
    shape_params.neighbour_range_pixels = px_range
    shape_params.nbins_z = 6
    shape_params.interpolation = ohkl.PeakInterpolation_InverseDistance
    shape_params.sigma_m = 0.26
    shape_params.sigma_d = 0.36

    shapes = ohkl.ShapeModel(shape_params);
    shapes.integrate(found_peaks.getPeakList(), data);
    shapes.setPredictedShapes(predicted_peaks)

    # Integration parameters
    integration_params = expt.integrator().parameters()
    integration_params.integrator_type = ohkl.IntegratorType_PixelSum
    integration_params.region_type = ohkl.RegionType_FixedEllipsoid
    integration_params.fixed_peak_end = 5.5
    integration_params.fixed_bkg_begin = 1.0
    integration_params.fixed_bkg_end = 2.3
    integration_params.discard_saturated = True
    integration_params.max_counts = 50000

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
    print("Neighbour range (pixels) = " + str(px_range))
    print(merger.summary())

    d = merger.getFigureOfMerit(ohkl.FigureOfMerit_d, ohkl.IntegratorType_PixelSum)
    rpim = merger.getFigureOfMerit(ohkl.FigureOfMerit_Rpim, ohkl.IntegratorType_PixelSum)
    cchalf = merger.getFigureOfMerit(ohkl.FigureOfMerit_CChalf, ohkl.IntegratorType_PixelSum)

    plt.plot(d, cchalf, label=f'Peak end = {peak_end}', linewidth=0.5)

plt.legend()
plt.savefig("trypsin_predicted_shape_pxrange.pdf")
