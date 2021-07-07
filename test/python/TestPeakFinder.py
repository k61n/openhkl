import sys
import glob
import unittest
import pynsx as nsx

class TestPeakFinder(unittest.TestCase):

    def test(self):
        # Initialise the experiment
        name = "FutA"
        detector = "BioDiff5000"
        expt = nsx.Experiment(name, detector)

        # Read the data
        files = glob.glob('p16106_00029*.tiff')
        if len(files) != 7:
            raise Exception(f"Found {len(files)} tiff files while expecting 7")
        data_params = nsx.RawDataReaderParameters()
        # instrument and file parameters
        data_params.wavelength = 3.1
        data_params.delta_omega = 0.4
        data_params.row_major = True
        data_params.swap_endian = True
        data_params.bpp = 2
        reader = nsx.RawDataReader(files[0], expt.getDiffractometer())
        reader.setParameters(data_params)
        for filename in files[0:]:
            reader.addFrame(filename)
        reader.end()
        expt.addData(nsx.DataSet(reader))

        # Find the peaks
        finder = expt.peakFinder()
        params = finder.parameters()
        params.frames_begin = 0
        params.frames_end = -1
        params.minimum_size = 30
        params.maximum_size = 10000
        params.peak_end = 1.0
        params.threshold = 80
        finder.setConvolver(nsx.AnnularConvolver())

        finder.find(expt.getAllData())

        # Integrate the peaks
        integrator = expt.integrator()
        integrator_params = integrator.parameters()
        integrator_params.peak_end = 3.0
        integrator_params.bkg_begin = 3.0
        integrator_params.bkg_end = 6.0
        integrator.integrateFoundPeaks(finder)
        expt.acceptFoundPeaks("found")
        found_peaks = expt.getPeakCollection("found")
        n_peaks = found_peaks.numberOfPeaks()
        self.assertTrue(n_peaks >= 210, f"found only {n_peaks} peaks")

if __name__ == "__main__":
    unittest.main()
