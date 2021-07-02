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
        expt.addData(nsx.DataSet(reader), name)

        # Find the peaks
        finder = expt.peakFinder()
        finder.setFramesBegin(0)
        finder.setFramesEnd(-1)
        finder.setConvolver(nsx.AnnularConvolver())
        finder.setMinSize(30)
        finder.setMaxSize(10000)
        finder.setPeakEnd(1.0)
        finder.setThreshold(80.0)
        finder.find(expt.getAllData())

        # Integrate the peaks
        integrator = expt.getIntegrator(nsx.IntegratorType_PixelSum)
        integrator_params = expt.int_params
        integrator_params.peak_end = 3.0
        integrator_params.bkg_begin = 3.0
        integrator_params.bkg_end = 6.0
        integrator.setParameters(integrator_params)
        expt.integrateFoundPeaks(nsx.IntegratorType_PixelSum)
        expt.acceptFoundPeaks("found")
        found_peaks = expt.getPeakCollection("found")
        n_peaks = found_peaks.numberOfPeaks()
        self.assertTrue(n_peaks >= 210, f"found only {n_peaks} peaks")

if __name__ == "__main__":
    unittest.main()
