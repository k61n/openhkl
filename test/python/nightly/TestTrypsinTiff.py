#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestTrypsinTiff.py
##! @brief     Test full workflow for trypsin
##!
##! @homepage  https://openhkl.org
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import unittest
from pathlib import Path
import pyohkl as ohkl

class TestFullWorkFlow(unittest.TestCase):

    def test(self):

        print('OpenHKL TestTrypsinTiff')
        data_dir = "."
        #detector_mask_file = "./trypsin_default_masks.yml"

        # set up the experiment
        expt = ohkl.Experiment('trypsin', 'BioDiff')
        diffractometer = expt.getDiffractometer()

        # This allows to choose between several detector resolutions for a given instrument
        # Up to 3 options are possible and choise 0 is the default value
        # for BioDiff this means 2500 cols is default value  (but also 5000 and 10k as option 2, 3)
        # diffractometer.detector().selectDetectorResolution(0)

        print ("> Using instrument: BioDiff" +  str(diffractometer.detector().nCols())) # check
        dataset = ohkl.DataSet('trypsin', diffractometer)

        # raw data parameters
        data_params = expt.tiff_params
        data_params.wavelength = 2.67
        data_params.delta_omega = 0.4

        print(f'Reading files from {data_dir}')
        dir = Path(data_dir)
        tiff_data_files = sorted(list(dir.glob('*.tiff')))

        dataset.setTiffReaderParameters(data_params)
        for filename in tiff_data_files:
            print (filename)
            dataset.addTiffFrame(str(filename))

        nfiles = len(tiff_data_files)
        self.assertTrue(nfiles == 169, f"found {nfiles} raw data files")

        dataset.finishRead()
        expt.addData(dataset)
        data = expt.getData('trypsin')

        # override machine baseline/gain for simulated data
        data.detector().setBaseline(0.0)
        data.detector().setGain(1.0)

        # this loads detector masks from given external file into dataset
        #mimporter = ohkl.MaskImporter(detector_mask_file, dataset.nFrames())
        #masks = mimporter.getMasks() #VectorIMasks
        #for i in range(0, len(masks)):
        #    dataset.addMask(masks[i])
        #    print ("added detector mask #", i)

        print('Finding peaks...')
        peak_finder = expt.peakFinder()
        params = peak_finder.parameters()
        params.threshold = 80
        peak_finder.find(data)
        print(f'Found {peak_finder.numberFound()} peaks')
        self.assertTrue(peak_finder.numberFound() == 9947)


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
        print(f'Integrated {found_peaks.numberOfValid()} valid peaks')
        self.assertTrue(found_peaks.numberOfValid() > 8420 and
                        found_peaks.numberOfValid() < 8550)


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
        expt.setReferenceCell(54.9, 58.4, 67.3, 90, 90, 90) # used to pick best solution
        space_group = ohkl.SpaceGroup('P 21 21 21') # required to check that Bravais type is correct
        reference_cell = expt.getUnitCell('reference')
        reference_cell.setSpaceGroup(space_group)
        # Adjust the direct beam position
        data.adjustDirectBeam(-1.8145, -3.9157) # manual values from GUI
        indexer = expt.autoIndexer();
        params = indexer.parameters();
        params.length_tol = 1.0
        params.angle_tol = 0.1
        # Even with adjusted direct beam position a much higher value
        # for params.nVertices is require to ensure the script does
        # find the unit cell
        # params.nVertices = 30000 # default is 10k

        indexing_peaks = expt.getPeakCollection('indexing')
        indexer.autoIndex(indexing_peaks)
        indexed_cell = indexer.goodSolution(reference_cell, 1, 0.1)
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
        expt.addPeakCollection('predicted', ohkl.PeakCollectionType_PREDICTED, predictor.peaks(), data)
        predicted_peaks = expt.getPeakCollection('predicted')
        print(f'{predicted_peaks.numberOfPeaks()} peaks predicted')
        self.assertTrue(predicted_peaks.numberOfPeaks() > 57720 and
                        predicted_peaks.numberOfPeaks() < 58750)

        print('Building shape model...')
        filtered_peaks.computeSigmas()
        params = ohkl.ShapeModelParameters()
        params.sigma_d = filtered_peaks.sigmaD()
        params.sigma_m = filtered_peaks.sigmaM()
        filtered_peaks.buildShapeModel(data, params)
        print(f'{filtered_peaks.shapeModel().numberOfPeaks()} shapes generated')
        self.assertTrue(filtered_peaks.shapeModel().numberOfPeaks() > 8240 and
                        filtered_peaks.shapeModel().numberOfPeaks() < 8260)

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
        self.assertTrue(integrator.numberOfValidPeaks() > 54300 and
                        integrator.numberOfValidPeaks() < 55750)

        print('Merging predicted peaks...')
        merger = expt.peakMerger()
        params = merger.parameters()
        merger.reset()
        params.d_min = 1.5
        merger.addPeakCollection(predicted_peaks)
        merger.mergePeaks()
        merger.computeQuality()

        # Target values for statistics in 3 lowest resolution shells
        # values of these two arrays have been adjusted for the project
        # with tiff files
        rpim_ref = [0.02633, 0.0527758, 0.121082]
        compl_ref = [0.963999, 0.96263, 0.960946]
        eps = 0.1

        for i in range(3):
            #print ("DBG ASSERT TRUE RPIM VALUE ", rpim_ref[i] + eps - merger.shellQuality().shells[i].Rpim)
            self.assertTrue(rpim_ref[i] + eps - merger.shellQuality().shells[i].Rpim > 0)
            #print ("DBG ASSERT TRUE COMPLT VALUE ", compl_ref[i] + eps - merger.shellQuality().shells[i].Completeness)
            self.assertTrue(compl_ref[i] + eps - merger.shellQuality().shells[i].Completeness > 0)

        print("Workflow complete")

if __name__ == '__main__':
    unittest.main()
