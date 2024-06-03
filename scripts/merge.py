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
from pathlib import Path

lib_dir = "@SWIG_INSTALL_PATH@"  # Path to pyohkl.py

sys.path.append(lib_dir)
import pyohkl as ohkl

file = Path('@CMAKE_BINARY_DIR@/test/data/Trypsin-pxsum.ohkl')
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
print(merger.summary())

merger.savePeaks("unmerged_sum.txt", True)
