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
import unittest
from pathlib import Path

data_dir = '/home/zamaan/projects/datasets/small_cell_low_intensity' # Path to .raw data files
lib_dir = '/home/zamaan/projects/nsxtool/build/swig' # Path to pnynsx.py

sys.path.append(lib_dir)
import pynsx as nsx

# set up the experiment
expt = nsx.Experiment('trypsin-sim', 'BioDiff2500')
diffractometer = expt.getDiffractometer()
dataset = nsx.DataSet('trypsin-sim', diffractometer)

# raw data parameters
data_params = expt.data_params
data_params.wavelength = 2.6
data_params.delta_omega = 0.3

print(f'Reading files from {data_dir}')
dir = Path(data_dir)
raw_data_files = sorted(list(dir.glob('*.raw')))
for file in raw_data_files:
    print(file)
dataset.setRawReaderParameters(data_params)
for filename in raw_data_files:
    dataset.addRawFrame(str(filename))

dataset.finishRead()
expt.addData(dataset)
data = expt.getData('trypsin-sim')

print('Finding peaks...')
peak_finder = expt.peakFinder()
params = peak_finder.parameters()
params.threshold = 80
peak_finder.find([data]) # argument is a list of pointers to data sets
print(f'Found {peak_finder.numberFound()} peaks')

print('Integrating found peaks...')
integrator = expt.integrator()
params = integrator.parameters()
params.peak_end = 3.0
params.bkg_begin = 3.0
params.bkg_end = 6.0
integrator.integrateFoundPeaks(peak_finder)
expt.acceptFoundPeaks('found') # Peak collection is now saved to experiment as "found"
found_peaks = expt.getPeakCollection('found')
print(f'Integrated {found_peaks.numberOfValid()} valid peaks')

print('Indexing found peaks...')
expt.setReferenceCell(24.5, 28.7, 37.7, 90, 90, 90) # reference cell used to pick best solution
space_group = nsx.SpaceGroup('P 21 21 21') # required to check that Bravais type is correct
reference_cell = expt.getUnitCell('reference')
reference_cell.setSpaceGroup(space_group)
indexer = expt.autoIndexer();
params = indexer.parameters();
params.length_tol = 1.5
params.angle_tol = 0.1

# Filter to generate the peak collection for indexing
filter = expt.peakFilter();
filter.resetFilterFlags();
filter.flags().strength = True;
filter.flags().d_range = True;
filter.flags().frames = True;
filter.parameters().d_min = 1.5
filter.parameters().frame_min = 10
filter.parameters().frame_max= 20
filter.parameters().strength_min = 1.0
filter.filter(found_peaks)
expt.acceptFilter('indexing', found_peaks)

indexing_peaks = expt.getPeakCollection('indexing')
indexer.autoIndex(indexing_peaks)
indexed_cell = indexer.goodSolution(reference_cell, 1.5, 0.2)
print(f'Reference cell {reference_cell.toString()}')
print(f'Using cell     {indexed_cell.toString()}')

print('Predicting peaks...')
cell = expt.getSptrUnitCell('indexing')
predictor = expt.predictor()
params = predictor.parameters()
params.d_min = 1.5
#predictor.predictPeaks(data, cell)
expt.predictPeaks('predicted', data, indexed_cell)
predicted_peaks = expt.getPeakCollection('predicted')
print(f'{predicted_peaks.numberOfPeaks()} peaks predicted')

print('Refining...')
refiner = expt.refiner()
params = refiner.parameters()
params.nbatches = 1
params.refine_ub = True
params.refine_sample_position = False
params.refine_sample_orientation = False
params.refine_detector_offset = False
params.refine_ki = False
params.residual_type = nsx.ResidualType_RealSpace
states = data.instrumentStates()
peak_list = found_peaks.getPeakList()
predicted_peak_list = predicted_peaks.getPeakList()


refiner.makeBatches(states, peak_list, indexed_cell)
refine_success = refiner.refine()
n_updated = refiner.updatePredictions(predicted_peak_list)
print(f'Refine 1: {n_updated} peaks updated')
params.residual_type = nsx.ResidualType_QSpace
refiner.makeBatches(states, peak_list, indexed_cell)
refine_success = refiner.refine()
n_updated = refiner.updatePredictions(predicted_peak_list)
print(f'Refine 2: {n_updated} peaks updated')
params.residual_type = nsx.ResidualType_RealSpace
refiner.makeBatches(states, peak_list, indexed_cell)
refine_success = refiner.refine()
n_updated = refiner.updatePredictions(predicted_peak_list)
print(f'Refine 3: {n_updated} peaks updated')

print('Filtering fit peaks for shape collection...')
found_peaks.computeSigmas()
filter = expt.peakFilter()
filter.resetFiltering(found_peaks)
filter.resetFilterFlags()
flags = filter.flags()
flags.d_range = True
flags.strength = True
params = filter.parameters()
params.d_min = 1.5
params.strength_min = 1.0
params.strength_max = 1000000.0
filter.filter(found_peaks)
filtered_peaks = nsx.PeakCollection('fit', nsx.listtype_FILTERED)
filtered_peaks.populateFromFiltered(found_peaks)


print('Building shape collection...')

shapes = nsx.ShapeCollection()
params = nsx.ShapeCollectionParameters()
aabb = nsx.AABB()
sigma = np.array([found_peaks.sigmaD(), found_peaks.sigmaD(), found_peaks.sigmaM()])
aabb.setLower(-params.peak_end * sigma)
aabb.setUpper(params.peak_end * sigma)
integrator = expt.integrator()
fit_peak_list = filtered_peaks.getPeakList()
integrator.integrateShapeCollection(fit_peak_list, data, shapes, aabb, params)
print(f'{shapes.numberOfPeaks()} shapes generated')

print('Assigning shapes to predicted peaks...')
interpolation = nsx.PeakInterpolation_InverseDistance
shapes.setPredictedShapes(predicted_peaks, interpolation)

print('Integrating predicted peaks...')
integrator = expt.integrator()
params = integrator.parameters()
integrator_type = nsx.IntegratorType_Profile3D
integrator.getIntegrator(integrator_type)
integrator.integratePeaks(data, predicted_peaks, params, shapes)
print(f'{integrator.numberOfValidPeaks()} / {integrator.numberOfPeaks()} peaks integrated')

print('Merging predicted peaks...')
merger = expt.peakMerger()
params = merger.parameters()
merger.reset()
merger.addPeakCollection(predicted_peaks)
merger.mergePeaks()
merger.computeQuality()


