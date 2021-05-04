import sys
import glob
import unittest
from pdb import set_trace
import pynsx as nsx

class TestExperimentFileIO(unittest.TestCase):

    def test(self):
        name = "FutA"
        detector = "BioDiff5000"

        # Reference data
        expt_ref = nsx.Experiment(name, detector)
        expt_ref.loadFromFile("FutA.nsx")
        expt_ref.saveToFile("test.nsx") # test ExperimentExporter
        found_peaks_ref = expt_ref.getPeakCollection("found")
        filtered_peaks_ref = expt_ref.getPeakCollection("filtered")
        npeaks_ref = found_peaks_ref.numberOfPeaks()
        ncaught_ref = filtered_peaks_ref.numberOfPeaks()
        unitcell_ref = expt_ref.getUnitCell("accepted")
        nframes_ref = expt_ref.getAllData()[0].nFrames()

        # Load HDF5 file and compare with reference
        expt_test = nsx.Experiment(name, detector)
        expt_test.loadFromFile("test.nsx") # test ExperimentImporter
        found_peaks_test = expt_test.getPeakCollection("found")
        filtered_peaks_test = expt_test.getPeakCollection("filtered")
        npeaks_test = found_peaks_test.numberOfPeaks()
        ncaught_test = filtered_peaks_test.numberOfPeaks()
        unitcell_test = expt_test.getUnitCell("accepted")
        nframes_test = expt_test.getAllData()[0].nFrames()

        # compare peak collections
        self.assertEqual(npeaks_test, npeaks_ref)
        self.assertEqual(ncaught_test, ncaught_ref)
        # compare unit cells
        self.assertTrue(unitcell_test.isSimilar(unitcell_ref, 0.01, 0.01))
        # compare data sets
        self.assertEqual(nframes_test, nframes_ref)

if __name__ == "__main__":
    unittest.main()
