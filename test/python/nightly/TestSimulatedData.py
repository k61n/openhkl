#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestSimulatedData.py
##! @brief     Test the full workflow using simulated data
##!
##! @homepage  ###HOMEPAGE###
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import unittest
import numpy as np
from math import isclose
from pathlib import Path
import pyohkl as ohkl
import os

class TestSimulatedData(unittest.TestCase):

    def test(self):
        # Reference values
        n_files = 60;
        ref_found_peaks = 2629
        ref_valid_found_peaks = 1911
        ref_predicted_peaks = 1955
        ref_shapes = 1725
        ref_valid_predicted_peaks = 1587

        # Numerical check thresholds
        eps_peaks = 10;
        eps_stat = 0.01;

        data_dir = '.' # Path to .raw data files

        # set up the experiment
        expt = ohkl.Experiment('trypsin-sim', 'BioDiff')
        diffractometer = expt.getDiffractometer()
        dataset = ohkl.DataSet('trypsin-sim', diffractometer)

        # raw data parameters
        data_params = expt.data_params
        data_params.wavelength = 2.67
        data_params.delta_omega = 0.3


        print(f'Reading files from {data_dir}')
        dir = Path(data_dir)
        raw_data_files = sorted(list(dir.glob('sim_small_cell*.raw')))
        # for file in raw_data_files:
            # print(file)
        dataset.setRawReaderParameters(data_params)
        for filename in raw_data_files:
            dataset.addFrame(str(filename), ohkl.DataFormat_RAW)

        self.assertEqual(len(raw_data_files), n_files, f"Found {len(raw_data_files)} files")

        dataset.finishRead()
        expt.addData(dataset)
        data = expt.getData('trypsin-sim')

        # override machine baseline/gain for simulated data
        data.detector().setBaseline(0.0)
        data.detector().setGain(1.0)

        print('Finding peaks...')
        peak_finder = expt.peakFinder()
        params = peak_finder.parameters()
        params.threshold = 80
        peak_finder.find(data)
        print(f'Found {peak_finder.numberFound()} peaks')
        self.assertEqual(
            peak_finder.numberFound(), ref_found_peaks, f"Found {peak_finder.numberFound()} peaks")

        print('Integrating found peaks...')
        integrator = expt.integrator()
        params = integrator.parameters()
        params.peak_end = 3.0
        params.bkg_begin = 3.0
        params.bkg_end = 6.0
        integrator.integrateFoundPeaks(peak_finder)
        expt.acceptFoundPeaks('found') # Peak collection is now saved to experiment as "found"
        expt.saveToFile("test.ohkl");
        found_peaks = expt.getPeakCollection('found')
        self.assertTrue(
            isclose(found_peaks.numberOfValid(), ref_valid_found_peaks, abs_tol=eps_peaks),
            f'Integrated {found_peaks.numberOfValid()} valid peaks')

        print('Indexing found peaks...')
        expt.setReferenceCell(24.5, 28.7, 37.7, 90, 90, 90, data) # reference cell used to pick best solution
        space_group = ohkl.SpaceGroup('P 21 21 21') # required to check that Bravais type is correct
        reference_cell = expt.getUnitCell('reference')
        reference_cell.setSpaceGroup(space_group)
        indexer = expt.autoIndexer();
        indexing_peaks = expt.getPeakCollection('found')
        indexer.autoIndex(indexing_peaks)
        indexed_cell = indexer.goodSolution(reference_cell, 1.5, 0.2)
        indexed_cell.setSpaceGroup(space_group)
        expt.addUnitCell("accepted", indexed_cell)
        expt.assignUnitCell(found_peaks, 'accepted');
        found_peaks.setMillerIndices();
        print(f'Reference cell {reference_cell.toString()}')
        print(f'Using cell     {indexed_cell.toString()}')

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
        filtered_peaks = ohkl.PeakCollection('fit', ohkl.PeakCollectionType_FOUND, data)
        filtered_peaks.populateFromFiltered(found_peaks)

        print('Predicting peaks...')
        cell = expt.getSptrUnitCell('indexing')
        predictor = expt.predictor()
        params = predictor.parameters()
        params.d_min = 1.5
        predictor.predictPeaks(data, indexed_cell)
        expt.addPeakCollection(
            'predicted', ohkl.PeakCollectionType_PREDICTED, predictor.peaks(), data, indexed_cell)
        predicted_peaks = expt.getPeakCollection('predicted')
        print()
        self.assertTrue(
            isclose(predicted_peaks.numberOfPeaks(), ref_predicted_peaks, abs_tol=eps_peaks),
            f'{predicted_peaks.numberOfPeaks()} peaks predicted')

        print('Building shape collection...')
        filtered_peaks.computeSigmas()
        params = ohkl.ShapeModelParameters()
        params.sigma_d = filtered_peaks.sigmaD()
        params.sigma_m = filtered_peaks.sigmaM()
        filtered_peaks.buildShapeModel(data, params)
        self.assertTrue(
            isclose(filtered_peaks.shapeModel().numberOfPeaks(), ref_shapes, abs_tol=eps_peaks),
            f'{filtered_peaks.shapeModel().numberOfPeaks()} shapes generated')

        print('Assigning shapes to predicted peaks...')
        filtered_peaks.shapeModel().setPredictedShapes(predicted_peaks)

        print('Refining...')
        refiner = expt.refiner()
        params = refiner.parameters()
        params.nbatches = 1
        params.refine_ub = True
        params.refine_sample_position = False
        params.refine_sample_orientation = False
        params.refine_detector_offset = False
        params.refine_ki = False
        params.residual_type = ohkl.ResidualType_RealSpace
        states = data.instrumentStates()
        peak_list = found_peaks.getPeakList()
        predicted_peak_list = predicted_peaks.getPeakList()


        params.residual_type = ohkl.ResidualType_RealSpace
        params.use_batch_cells = False
        refiner.makeBatches(states, peak_list, indexed_cell)
        refine_success = refiner.refine()
        n_updated = refiner.updatePredictions(predicted_peak_list)
        print(f'Refine 1: {n_updated} peaks updated')
        params.residual_type = ohkl.ResidualType_QSpace
        params.use_batch_cells = True
        refiner.makeBatches(states, peak_list, indexed_cell)
        refine_success = refiner.refine()
        n_updated = refiner.updatePredictions(predicted_peak_list)
        print(f'Refine 2: {n_updated} peaks updated')
        params.residual_type = ohkl.ResidualType_RealSpace
        params.use_batch_cells = True
        refiner.makeBatches(states, peak_list, indexed_cell)
        refine_success = refiner.refine()
        n_updated = refiner.updatePredictions(predicted_peak_list)
        print(f'Refine 3: {n_updated} peaks updated')

        print('Integrating predicted peaks...')
        integrator = expt.integrator()
        params = integrator.parameters()
        integrator_type = ohkl.IntegratorType_PixelSum
        integrator.getIntegrator(integrator_type)
        integrator.integratePeaks(data, predicted_peaks, params, filtered_peaks.shapeModel())
        self.assertTrue(
            isclose(integrator.numberOfValidPeaks(), ref_valid_predicted_peaks, abs_tol=eps_peaks),
            f'{integrator.numberOfValidPeaks()} / {integrator.numberOfPeaks()} peaks integrated')

        print('Merging predicted peaks...')
        predicted_peaks.setUnitCell(indexed_cell, False)
        merger = expt.peakMerger()
        params = merger.parameters()
        merger.reset()
        params.d_min = 1.5
        params.d_max = 50.0
        merger.addPeakCollection(predicted_peaks)
        merger.setSpaceGroup(space_group)
        merger.mergePeaks()
        merger.computeQuality()
        self.assertTrue(
            isclose(merger.sumOverallQuality().shells[0].Rpim, 0.0039, abs_tol=eps_stat),
            f'Overall Rpim = {merger.sumOverallQuality().shells[0].Rpim}')
        self.assertTrue(
            isclose(merger.sumOverallQuality().shells[0].Completeness, 0.185, abs_tol=eps_stat),
            f'Overall completeness = {merger.sumOverallQuality().shells[0].Completeness}')

        print("Workflow complete")

if __name__ == '__main__':
    unittest.main()
