import sys
import glob
import unittest
import pynsx as nsx

class TestPrediction(unittest.TestCase):

    def test(self):
        # Initialise the experiment
        name = "FutA"
        detector = "BioDiff5000"
        expt = nsx.Experiment(name, detector)

        expt.loadFromFile("FutA-2.nsx")
        found_peaks = expt.getPeakCollection("peaks")
        filtered_peaks = expt.getPeakCollection("filtered")
        expt.assignUnitCell(found_peaks)
        expt.assignUnitCell(filtered_peaks)

        # build the shape library
        data = expt.getAllData()
        shapelib_params = expt.shape_params
        shapelib_params.kabsch = True
        shapelib_params.sigma_m = 0.23
        shapelib_params.sigma_d = 0.33
        shapelib_params.nx = 20
        shapelib_params.ny = 20
        shapelib_params.nz = 20
        shapelib_params.peak_end = 3.0
        shapelib_params.d_min = 1.5
        shapelib_params.d_max = 50.0
        shapelib_params.bkg_begin = 3.0
        shapelib_params.bkg_end = 6.0
        expt.buildShapeCollection(filtered_peaks, shapelib_params)
        nprofiles = filtered_peaks.shapeCollection().numberOfPeaks()
        self.assertEqual(nprofiles, 132)

        # predict the peaks
        prediction_params = expt.predict_params
        prediction_params.d_min = 1.5
        prediction_params.d_max = 50.0
        prediction_params.neighbour_max_radius = 400.0
        prediction_params.frame_range_max = 20.0
        prediction_params.peak_end = 3.0
        prediction_params.bkg_begin = 3.0
        prediction_params.bkg_end = 6.0
        prediction_params.fit_center = True
        prediction_params.fit_covariance = True
        prediction_params.min_neighbours = 400.0
        expt.predictPeaks("predicted", filtered_peaks, prediction_params,
                        nsx.PeakInterpolation_NoInterpolation)
        predicted_peaks = expt.getPeakCollection("predicted")
        expt.integratePredictedPeaks('1d profile integrator', predicted_peaks,
                                    filtered_peaks.shapeCollection(), prediction_params)

        self.assertTrue(predicted_peaks.numberOfPeaks() > 200);
        # N.B. These results are not meaningful! The data set is too small to
        # produce a big enough shape library to integrate the predicted peaks,
        # so only a few predicted peaks are added. This should be changed in
        # the future - zamaan

if __name__ == "__main__":
    unittest.main()
