#!/usr/bin/env python3

import sys
import glob
import unittest
import pyohkl as ohkl

class TestPrediction(unittest.TestCase):

    def test(self):
        # Initialise the experiment
        name = "CrChiA"
        detector = "BioDiff2500"
        expt = ohkl.Experiment(name, detector)

        expt.loadFromFile("CrChiA.ohkl")
        found_peaks = expt.getPeakCollection("found")
        cell = expt.getUnitCell("accepted")
        data = expt.getData("testdata")

        # predict the peaks
        predictor = expt.predictor()
        prediction_params = predictor.parameters()
        prediction_params.d_min = 1.5
        prediction_params.d_max = 50.0
        predictor.predictPeaks(data, cell)
        expt.addPeakCollection("predicted", ohkl.PeakCollectionType_PREDICTED, predictor.peaks())
        predicted_peaks = expt.getPeakCollection("predicted")
        npeaks = predicted_peaks.numberOfPeaks()
        print(f'{npeaks} predicted')

        self.assertTrue(predicted_peaks.numberOfPeaks() > 5020);
        # N.B. These results are not meaningful! The data set is too small to
        # produce a big enough shape library to integrate the predicted peaks,
        # so only a few predicted peaks are added. This should be changed in
        # the future - zamaan

if __name__ == "__main__":
    unittest.main()
