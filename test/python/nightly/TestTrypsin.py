#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestTrypsin.py
##! @brief     Test full workflow for trypsin
##!
##! @homepage  https://openhkl.org
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import unittest
import numpy as np
from pathlib import Path
from math import isclose
import pyohkl as ohkl

class TestFullWorkFlow(unittest.TestCase):

    def test(self):
        # Reference values
        n_files = 169;
        ref_found_peaks = 25377;
        ref_found_integrated_peaks = 16927;
        ref_shapes = 16855;
        ref_predicted_peaks = 58155;
        ref_valid_predicted_peaks = 58215;
        ref_updated = 56750;
        ref_integrated = 52060;
        ref_rpim = [
            0.0264, 0.0584, 0.0954, 0.1350, 0.1705, 0.2159, 0.2527, 0.3024, 0.3714, 0.5312];
        ref_completeness = [
            0.9532, 0.8948, 0.6436, 0.6286, 0.7015, 0.6854, 0.6621, 0.6551, 0.6276, 0.5159];

        # Numerical check thresholds
        eps_peaks = 10;
        eps_stat = 0.01;

        print('OpenHKL TestTrypsin')
        data_dir = '.' # Path to .raw data files

        # set up the experiment
        expt = ohkl.Experiment('trypsin', 'BioDiff')
        diffractometer = expt.getDiffractometer()
        dataset = ohkl.DataSet('trypsin', diffractometer)

        # raw data parameters
        data_params = ohkl.DataReaderParameters()
        data_params.wavelength = 2.67
        data_params.delta_omega = 0.4
        data_params.data_format = ohkl.DataFormat_RAW
        data_params.rows = 900
        data_params.cols = 2500


        print(f'Reading files from {data_dir}')
        dir = Path(data_dir)
        raw_data_files = sorted(list(dir.glob('soak_9_d2*.raw')))
        dataset.setImageReaderParameters(data_params)
        for filename in raw_data_files:
            dataset.addRawFrame(str(filename))

        self.assertEqual(len(raw_data_files), n_files, f"found {n_files} raw data files")

        dataset.finishRead()
        expt.addData(dataset)
        data = expt.getData('trypsin')
        data.initBuffer(True)

        # add masks
        box_left = ohkl.AABB(
            np.array([0, 0, 0], dtype=float), np.array([250, 900, 169], dtype=float))
        box_right = ohkl.AABB(
            np.array([2250, 0, 0], dtype=float), np.array([2500, 900, 169], dtype=float))
        box_seam = ohkl.AABB(
            np.array([1725, 0, 0], dtype=float), np.array([1740, 900, 169], dtype=float))
        circle = ohkl.AABB(
            np.array([1200, 400, 0], dtype=float), np.array([1300, 500, 169], dtype=float))
        data.addBoxMask(box_left)
        data.addBoxMask(box_right)
        data.addBoxMask(box_seam)
        data.addEllipseMask(circle)

        # override yaml file baseline/gain for simulated data
        data.detector().setBaseline(0.0)
        data.detector().setGain(1.0)

        print('Finding peaks...')
        peak_finder = expt.peakFinder()
        params = peak_finder.parameters()
        params.threshold = 30
        peak_finder.find(data)
        self.assertTrue(isclose(peak_finder.numberFound(), ref_found_peaks, abs_tol=eps_peaks),
                        f'Found {peak_finder.numberFound()} peaks')

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
            isclose(found_peaks.numberOfValid(), ref_found_integrated_peaks, abs_tol=eps_peaks),
            f'Integrated {found_peaks.numberOfValid()} valid peaks')


        print('Filter peaks for indexing...')
        filter = expt.peakFilter();
        filter.resetFilterFlags();
        filter.flags().strength = True;
        filter.flags().d_range = True;
        filter.flags().frames = True;
        filter.parameters().d_min = 1.5
        filter.parameters().frame_min = 0
        filter.parameters().frame_max= 10
        filter.parameters().strength_min = 1.0
        filter.filter(found_peaks)
        expt.acceptFilter('indexing', found_peaks, ohkl.PeakCollectionType_INDEXING, data)


        print('Indexing found peaks...')
        expt.setReferenceCell(54.9, 58.4, 67.3, 90, 90, 90, data) # used to pick best solution
        space_group = ohkl.SpaceGroup('P 21 21 21') # required to check that Bravais type is correct
        reference_cell = expt.getUnitCell('reference')
        reference_cell.setSpaceGroup(space_group)
        # Adjust the direct beam position
        data.adjustDirectBeam(-1.0, -2.0)
        indexer = expt.autoIndexer();
        params = indexer.parameters();
        params.length_tol = 1.0
        params.angle_tol = 0.1

        indexing_peaks = expt.getPeakCollection('indexing')
        indexer.autoIndex(indexing_peaks)
        indexed_cell = indexer.goodSolution(reference_cell, 1.0, 0.1)
        indexed_cell.setSpaceGroup(space_group)
        expt.addUnitCell("accepted", indexed_cell)
        expt.assignUnitCell(found_peaks, 'accepted');
        found_peaks.setMillerIndices();
        cell = expt.getUnitCell("accepted")
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
        params.strength_min = 3.0
        params.strength_max = 1000000.0
        filter.filter(found_peaks)
        filtered_peaks = ohkl.PeakCollection('fit', ohkl.PeakCollectionType_FOUND, data)
        filtered_peaks.populateFromFiltered(found_peaks)


        print('Predicting peaks...')
        cell = expt.getSptrUnitCell('indexing')
        predictor = expt.predictor()
        params = predictor.parameters()
        params.d_min = 1.5
        params.d_max = 50.0
        predictor.predictPeaks(data, indexed_cell)
        expt.addPeakCollection(
            'predicted', ohkl.PeakCollectionType_PREDICTED, predictor.peaks(), data, cell)
        predicted_peaks = expt.getPeakCollection('predicted')
        self.assertTrue(
            isclose(predicted_peaks.numberOfPeaks(), ref_predicted_peaks, abs_tol=eps_peaks),
            f'{predicted_peaks.numberOfPeaks()} peaks predicted')


        print('Building shape model...')
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
        params.nbatches = 25
        params.refine_ub = True
        params.refine_sample_position = True
        params.refine_sample_orientation = True
        params.refine_detector_offset = True
        params.refine_ki = True
        params.use_batch_cells = False
        params.residual_type = ohkl.ResidualType_QSpace
        states = data.instrumentStates()
        peak_list = found_peaks.getPeakList()
        predicted_peak_list = predicted_peaks.getPeakList()
        refiner.makeBatches(states, peak_list, indexed_cell)
        refine_success = refiner.refine()
        n_updated = refiner.updatePredictions(predicted_peak_list)
        print(f'Refine 1: {n_updated} peaks updated')


        print('Integrating predicted peaks...')
        integrator = expt.integrator()
        params = integrator.parameters()
        params.integrator_type = ohkl.IntegratorType_PixelSum

        integrator.integratePeaks(data, predicted_peaks, params, filtered_peaks.shapeModel())
        print(f'{integrator.numberOfValidPeaks()} / {integrator.numberOfPeaks()} peaks integrated')
        self.assertTrue(
            isclose(integrator.numberOfValidPeaks(), ref_integrated, abs_tol=eps_peaks),
            f'{integrator.numberOfValidPeaks()} peaks successfully integrated')

        print('Merging predicted peaks...')
        predicted_peaks.setUnitCell(indexed_cell, False)
        merger = expt.peakMerger()
        params = merger.parameters()
        merger.reset()
        params.d_min = 1.5
        params.d_max = 50
        merger.addPeakCollection(predicted_peaks)
        merger.setSpaceGroup(space_group)
        merger.mergePeaks()
        merger.computeQuality()

        # Check Rpim and completeness against reference values
        for i in range(len(ref_rpim)):
            self.assertTrue(isclose(
                ref_rpim[i], merger.sumShellQuality().shells[i].Rpim, abs_tol=eps_stat))
            self.assertTrue(isclose(
                ref_completeness[i], merger.sumShellQuality().shells[i].Completeness, abs_tol=eps_stat))

        print("Workflow complete")

if __name__ == '__main__':
    unittest.main()
