#!/usr/bin/python
##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single-crystal diffraction
##
##! @file      scripts/generate_test_ohkl.py
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
import shutil

lib_dir = "/home/zamaan/projects/openhkl/build/swig/"  # Path to pyohkl.py

sys.path.append(lib_dir)
import pyohkl as ohkl

data_dir = '/home/zamaan/projects/datasets/CrChiA/'
files = ("CrChiA_c01runab_28603.raw",
         "CrChiA_c01runab_28604.raw",
         "CrChiA_c01runab_28605.raw",
         "CrChiA_c01runab_28606.raw",
         "CrChiA_c01runab_28607.raw",
         "CrChiA_c01runab_28608.raw",
         "CrChiA_c01runab_28609.raw",
         "CrChiA_c01runab_28610.raw",
         "CrChiA_c01runab_28611.raw",
         "CrChiA_c01runab_28612.raw",
         "CrChiA_c01runab_28613.raw",
         "CrChiA_c01runab_28614.raw",
         "CrChiA_c01runab_28615.raw",
         "CrChiA_c01runab_28616.raw",
         "CrChiA_c01runab_28617.raw")

# Initialise the experiment
name = "CrChiA"
detector = "BioDiff2500"
expt = ohkl.Experiment(name, detector)

dataset = ohkl.DataSet("testdata", expt.getDiffractometer())
# instrument and file parameters
data_params = expt.data_params
data_params.wavelength = 2.669
data_params.delta_omega = 0.3
data_params.row_major = True
data_params.swap_endian = True
data_params.bpp = 2
dataset.setRawReaderParameters(data_params)
for filename in files:
    shutil.copyfile(data_dir + filename, filename)
    dataset.addRawFrame(data_dir + filename)

dataset.finishRead()
expt.addData(dataset)

# Find the peaks
finder = expt.peakFinder()
params = finder.parameters()
params.frames_begin = 0
params.frames_end = -1
params.minimum_size = 30
params.maximum_size = 10000
params.peak_end = 1.0
params.threshold = 80
finder.setConvolver(ohkl.AnnularConvolver())

finder.find(expt.getAllData())

# Integrate the peaks
integrator = expt.integrator()
integrator_params = integrator.parameters()
integrator_params.peak_end = 3.0
integrator_params.bkg_begin = 3.0
integrator_params.bkg_end = 6.0
integrator_params.use_gradient = True;
integrator_params.gradient_type = ohkl.GradientKernel_Sobel;
integrator_params.fft_gradient = False;
integrator.integrateFoundPeaks(finder)
expt.acceptFoundPeaks("found")
found_peaks = expt.getPeakCollection("found")
n_peaks = found_peaks.numberOfPeaks()
print("Find peaks: ", n_peaks, " found")

filter = expt.peakFilter()
filter.flags().strength = True
filter.flags().d_range = True
filter.parameters().d_min = 1.5
filter.parameters().d_max = 50.0
filter.parameters().strength_min = 1.0
filter.parameters().strength_max = 1.0e7
filter.filter(found_peaks)
expt.acceptFilter("filtered", found_peaks, ohkl.PeakCollectionType_FOUND)
filtered_peaks = expt.getPeakCollection("filtered")
n_caught = filtered_peaks.numberCaughtByFilter()
print(f'Autoindex: {n_caught}/{n_peaks} peaks caught by filter')

# Autoindex the peaks
autoindexer = expt.autoIndexer()
expt.setReferenceCell(57.96, 65.12, 86.52, 90.0, 90.0, 90.0)
autoindexer_params = ohkl.IndexerParameters()
autoindexer_params.maxdim = 100.0
autoindexer_params.nSolutions = 10
autoindexer_params.nVertices = 10000
autoindexer_params.subdiv = 30
autoindexer_params.indexingTolerance = 0.2
autoindexer_params.minUnitCellVolume = 10000.0
autoindexer.autoIndex(filtered_peaks)
reference_cell = expt.getReferenceCell()
reference_cell.setSpaceGroup(ohkl.SpaceGroup("P 21 21 21"))
print(autoindexer.solutionsToString())
print(expt.checkAndAssignUnitCell(found_peaks, 2.0, 0.1))  # boolean return value
print(expt.checkAndAssignUnitCell(filtered_peaks, 2.0, 0.1))  # boolean return value

# Predict peaks
predictor = expt.predictor()
data = expt.getAllData()[0]
indexed_cell = expt.getUnitCell("accepted")
prediction_params = predictor.parameters()
prediction_params.d_min = 1.5
prediction_params.d_max = 50.0
predictor.predictPeaks(data, indexed_cell)
expt.addPeakCollection("predicted", ohkl.PeakCollectionType_PREDICTED, predictor.peaks())
predicted_peaks = expt.getPeakCollection("predicted")
npeaks = predicted_peaks.numberOfPeaks()
print(f'{npeaks} predicted')

# Generate shapes
found_peaks.computeSigmas()
params = ohkl.ShapeModelParameters()
params.sigma_d = found_peaks.sigmaD()
params.sigma_m = found_peaks.sigmaM()
found_peaks.buildShapeModel(data, params)
print(f'{found_peaks.shapeModel().numberOfPeaks()} shapes generated')
shapes = found_peaks.shapeModel()

# Assign shapes to predicted peaks
interpolation = ohkl.PeakInterpolation_InverseDistance
shapes.setPredictedShapes(predicted_peaks)

expt.saveToFile(f'{name}.ohkl')
