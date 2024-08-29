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

lib_dir = "@SWIG_INSTALL_PATH@"  # Path to pynsx.py

sys.path.append(lib_dir)
import pynsx as nsx

############################################################################
# Modifiy these according to your data set
name = 'CrChiA'
data_dir = '/home/zamaan/projects/datasets/CrChiA'  # Path to .raw data files
diffractometer = 'BioDiff'
wl = 2.67
d_omega = 0.3
a = 57.96
b = 65.12
c = 86.52
alpha = 90.0
beta = 90.0
gamma = 90.0
sg = 'P 21 21 21'
predicted_d_min = 1.5
############################################################################


def load_data(experiment, name, diffractometer, path):
    print(f'Reading files from {data_dir}...')
    dataset = nsx.DataSet(name, diffractometer)
    data_params = expt.data_params
    data_params.wavelength = wl
    data_params.delta_omega = d_omega
    dir = Path(data_dir)
    raw_data_files = sorted(list(dir.glob('*.raw')))
    for file in raw_data_files:
        print(file)
    print(f'{len(raw_data_files)} files read')
    dataset.setRawReaderParameters(data_params)
    for filename in raw_data_files:
        dataset.addFrame(str(filename), nsx.DataFormat_RAW)

    dataset.finishRead()
    experiment.addData(dataset)


def find_peaks(experiment, threshold):
    print('Finding peaks...')
    peak_finder = experiment.peakFinder()
    params = peak_finder.parameters()
    params.threshold = threshold
    peak_finder.find(experiment.getAllData())
    print(f'Found {peak_finder.numberFound()} peaks')

    print('Integrating found peaks...')
    integrator = experiment.integrator()
    params = integrator.parameters()
    params.peak_end = 3.0
    params.bkg_begin = 3.0
    params.bkg_end = 6.0
    integrator.integrateFoundPeaks(peak_finder)
    experiment.acceptFoundPeaks('found') # Peak collection is now saved to experiment as "found"
    found_peaks = experiment.getPeakCollection('found')
    print(f'Integrated {found_peaks.numberOfValid()} valid peaks')
    return found_peaks


def index(experiment, a, b, c, alpha, beta, gamma, sg):
    print('Indexing found peaks...')
    experiment.setReferenceCell(a, b, c, alpha, beta, gamma) # reference cell used to pick best solution
    space_group = nsx.SpaceGroup(sg) # required to check that Bravais type is correct
    reference_cell = experiment.getUnitCell('reference')
    reference_cell.setSpaceGroup(space_group)
    indexer = experiment.autoIndexer()
    params = indexer.parameters()
    params.length_tol = 1.5
    params.angle_tol = 0.1

    # Filter to generate the peak collection for indexing
    filter = experiment.peakFilter()
    filter.resetFilterFlags()
    filter.flags().strength = True
    filter.flags().d_range = True
    filter.flags().frames = True
    filter.parameters().d_min = 1.5
    filter.parameters().first_frame = 10
    filter.parameters().last_frame = 20
    filter.parameters().strength_min = 1.0
    filter.filter(found_peaks)
    experiment.acceptFilter('indexing', found_peaks)

    indexing_peaks = experiment.getPeakCollection('indexing')
    indexer.autoIndex(indexing_peaks)
    print(indexer.solutionsToString())
    indexed_cell = indexer.goodSolution(reference_cell, 1.5, 0.2)
    indexed_cell.setSpaceGroup(space_group)
    experiment.addUnitCell("accepted", indexed_cell)
    experiment.assignUnitCell(found_peaks, 'accepted')
    print(f'Reference cell {reference_cell.toString()}')
    print(f'Using cell     {indexed_cell.toString()}')
    return experiment.getUnitCell("accepted")


def predict(experiment, cell, data, d_min):
    predictor = experiment.predictor()
    params = predictor.parameters()
    params.d_min = d_min
    print()
    predictor.predictPeaks(data, cell)
    experiment.addPeakCollection('predicted', nsx.PeakCollectionType_PREDICTED, predictor.peaks())
    predicted_peaks = experiment.getPeakCollection('predicted')
    print(f'{predicted_peaks.numberOfPeaks()} peaks predicted')
    return predicted_peaks


def get_shapes(data, peaks):
    peaks.computeSigmas()
    params = nsx.ShapeModelParameters()
    params.sigma_d = peaks.sigmaD()
    params.sigma_m = peaks.sigmaM()
    peaks.buildShapeModel(data, params)
    print(f'{peaks.shapeCollection().numberOfPeaks()} shapes generated')
    return peaks.shapeCollection()


expt = nsx.Experiment(name, diffractometer)
d = expt.getDiffractometer()

# find peaks from raw images (if necessary)
found_peaks = None
found_peaks_file = Path(f'{name}-found.nsx')
if found_peaks_file.exists():
    expt.loadFromFile(str(found_peaks_file))
    found_peaks = expt.getPeakCollection('found')
else:
    load_data(expt, name, d, data_dir)
    found_peaks = find_peaks(expt, 80)
    expt.saveToFile(str(found_peaks_file))

data = expt.getData(name)

indexed_cell = index(expt, a, b, c, alpha, beta, gamma, sg)

print('Filtering fit peaks for shape collection...')
filter = expt.peakFilter()
filter.resetFiltering(found_peaks)
filter.resetFilterFlags()
flags = filter.flags()
flags.d_range = True
flags.strength = True
params = filter.parameters()
params.d_min = 1.5
params.strength_min = 10.0
params.strength_max = 1000000.0
filter.filter(found_peaks)
filtered_peaks = nsx.PeakCollection('fit', nsx.PeakCollectionType_FOUND)
filtered_peaks.populateFromFiltered(found_peaks)

print('Refining direct beam positions...')
refiner = expt.refiner()
params = refiner.parameters()
params.nbatches = data.nFrames()
params.refine_ub = False
params.refine_sample_position = False
params.refine_sample_orientation = False
params.refine_detector_offset = False
params.refine_ki = True
params.residual_type = nsx.ResidualType_RealSpace
params.use_batch_cells = False
states = data.instrumentStates()
peak_list = found_peaks.getPeakList()
refiner.makeBatches(states, peak_list, indexed_cell)
refine_success = refiner.refine()

print('Predicting peaks...')
predicted_peaks = predict(expt, indexed_cell, data, predicted_d_min)

print('Building shape collection...')
shapes = get_shapes(data, filtered_peaks)

print('Assigning shapes to predicted peaks...')
interpolation = nsx.PeakInterpolation_InverseDistance
shapes.setPredictedShapes(predicted_peaks)

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

params.residual_type = nsx.ResidualType_RealSpace
params.use_batch_cells = False
refiner.makeBatches(states, peak_list, indexed_cell)
refine_success = refiner.refine()
n_updated = refiner.updatePredictions(predicted_peak_list)
print(f'Refine 1: {n_updated} peaks updated')
params.residual_type = nsx.ResidualType_QSpace
params.use_batch_cells = True
refiner.makeBatches(states, peak_list, indexed_cell)
refine_success = refiner.refine()
n_updated = refiner.updatePredictions(predicted_peak_list)
print(f'Refine 2: {n_updated} peaks updated')
params.residual_type = nsx.ResidualType_RealSpace
params.use_batch_cells = True
refiner.makeBatches(states, peak_list, indexed_cell)
refine_success = refiner.refine()
n_updated = refiner.updatePredictions(predicted_peak_list)
print(f'Refine 3: {n_updated} peaks updated')

print('Integrating predicted peaks...')
integrator = expt.integrator()
params = integrator.parameters()
params.neighbour_range_pixels = 500
params.neighbour_range_frames = 5
integrator_type = nsx.IntegratorType_Profile3D
integrator.getIntegrator(integrator_type)
integrator.integratePeaks(data, predicted_peaks, params, filtered_peaks.shapeCollection())
print(f'{integrator.numberOfValidPeaks()} / {integrator.numberOfPeaks()} peaks integrated')

print('Merging predicted peaks...')
merger = expt.peakMerger()
params = merger.parameters()
merger.reset()
params.d_min = 1.5
params.first_frame = 1  # exclude first and last frames from statistics
params.last_frame = data.nFrames() - 1
merger.addPeakCollection(predicted_peaks)
merger.mergePeaks()
merger.computeQuality()

print("Workflow complete")

expt.saveToFile(f'{name}.nsx')
