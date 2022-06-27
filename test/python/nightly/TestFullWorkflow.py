##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestFullWorkFlow.py
##! @brief     Test ...
##!
##! @homepage  ###HOMEPAGE###
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import numpy as np
import unittest
from pathlib import Path
import pyohkl as ohkl
import os

class TestFullWorkFlow(unittest.TestCase):

    def test(self):
        print()
        data_dir = 'small_cell_low_intensity' # Path to .raw data files

        # set up the experiment
        expt = ohkl.Experiment('trypsin-sim', 'BioDiff2500')
        diffractometer = expt.getDiffractometer()
        dataset = ohkl.DataSet('trypsin-sim', diffractometer)

        # raw data parameters
        data_params = expt.data_params
        data_params.wavelength = 2.67
        data_params.delta_omega = 0.3

        print(f'Reading files from {data_dir}')
        dir = Path(data_dir)
        raw_data_files = sorted(list(dir.glob('*.raw')))
        # for file in raw_data_files:
            # print(file)
        dataset.setRawReaderParameters(data_params)
        for filename in raw_data_files:
            dataset.addRawFrame(str(filename))

        self.assertTrue(len(raw_data_files) == 60)

        dataset.finishRead()
        expt.addData(dataset)
        data = expt.getData('trypsin-sim')

        print('Finding peaks...')
        peak_finder = expt.peakFinder()
        params = peak_finder.parameters()
        params.threshold = 80
        peak_finder.find(expt.getAllData())
        print(f'Found {peak_finder.numberFound()} peaks')
        self.assertTrue(peak_finder.numberFound() == 2629)

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
        self.assertTrue(found_peaks.numberOfValid() > 1900 and
                        found_peaks.numberOfValid() < 1920)

        print('Indexing found peaks...')
        expt.setReferenceCell(24.5, 28.7, 37.7, 90, 90, 90) # reference cell used to pick best solution
        space_group = ohkl.SpaceGroup('P 21 21 21') # required to check that Bravais type is correct
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
        expt.acceptFilter('indexing', found_peaks, ohkl.PeakCollectionType_INDEXING)        

        indexing_peaks = expt.getPeakCollection('indexing')
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
        filtered_peaks = ohkl.PeakCollection('fit', ohkl.PeakCollectionType_FOUND)
        filtered_peaks.populateFromFiltered(found_peaks)

        print('Predicting peaks...')
        cell = expt.getSptrUnitCell('indexing')
        predictor = expt.predictor()
        params = predictor.parameters()
        params.d_min = 1.5
        predictor.predictPeaks(data, indexed_cell)
        expt.addPeakCollection('predicted', ohkl.PeakCollectionType_PREDICTED, predictor.peaks())
        predicted_peaks = expt.getPeakCollection('predicted')
        print(f'{predicted_peaks.numberOfPeaks()} peaks predicted')
        self.assertTrue(predicted_peaks.numberOfPeaks() > 1940 and
                        predicted_peaks.numberOfPeaks() < 1960)

        print('Building shape collection...')
        filtered_peaks.computeSigmas()
        params = ohkl.ShapeModelParameters()
        params.sigma_d = filtered_peaks.sigmaD()
        params.sigma_m = filtered_peaks.sigmaM()
        filtered_peaks.buildShapeModel(data, params)
        print(f'{filtered_peaks.shapeModel().numberOfPeaks()} shapes generated')
        self.assertTrue(filtered_peaks.shapeModel().numberOfPeaks() > 1720 and
                        filtered_peaks.shapeModel().numberOfPeaks() < 1740)

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
        integrator_type = ohkl.IntegratorType_Profile3D
        integrator.getIntegrator(integrator_type)
        integrator.integratePeaks(data, predicted_peaks, params, filtered_peaks.shapeModel())
        print(f'{integrator.numberOfValidPeaks()} / {integrator.numberOfPeaks()} peaks integrated')
        self.assertTrue(integrator.numberOfValidPeaks() >  1570 and
                        integrator.numberOfValidPeaks() < 1590)

        print('Merging predicted peaks...')
        merger = expt.peakMerger()
        params = merger.parameters()
        merger.reset()
        params.d_min = 1.5
        params.frame_min = 3
        params.frame_max = 58
        merger.addPeakCollection(predicted_peaks)
        merger.mergePeaks()
        merger.computeQuality()
        print(merger.overallQuality().shells[0].Rpim)
        self.assertTrue(merger.overallQuality().shells[0].Rpim < 0.11)

        print("Workflow complete")

if __name__ == '__main__':
    unittest.main()
