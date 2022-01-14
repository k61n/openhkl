import sys
import glob
import unittest
import pynsx as nsx

class TestAutoIndexer(unittest.TestCase):

    def test(self):
        # Initialise the experiment
        name = "FutA"
        detector = "BioDiff5000"
        expt = nsx.Experiment(name, detector)

        expt.loadFromFile("FutA-4.nsx")
        found_peaks = expt.getPeakCollection("peaks")
        n_peaks = found_peaks.numberOfPeaks()
        self.assertEqual(n_peaks, 269)

        # Filter the peaks
        filter = expt.peakFilter()
        filter.flags().strength = True
        filter.flags().d_range = True;
        filter.parameters().d_min = 1.55
        filter.parameters().d_max = 50.0
        filter.parameters().strength_min = 1.0
        filter.parameters().strength_max = 1.0e7
        filter.filter(found_peaks)
        expt.acceptFilter("filtered", found_peaks)
        filtered_peaks = expt.getPeakCollection("filtered")
        n_caught = filtered_peaks.numberCaughtByFilter()
        self.assertEqual(n_caught, 150)
        print(f'Autoindex: {n_caught}/{n_peaks} peaks caught by filter')

        # Autoindex the peaks
        autoindexer = expt.autoIndexer()
        expt.setReferenceCell(39.66, 78.19, 47.02, 90.0, 97.358, 90.0)
        autoindexer_params = nsx.IndexerParameters()
        autoindexer_params.maxdim = 200.0
        autoindexer_params.nSolutions = 15
        autoindexer_params.nVertices = 2300
        autoindexer_params.subdiv = 30
        autoindexer_params.indexingTolerance = 0.2
        autoindexer_params.minUnitCellVolume = 100.0
        autoindexer.autoIndex(filtered_peaks)
        reference_cell = expt.getReferenceCell()
        reference_cell.setSpaceGroup(nsx.SpaceGroup("P 21"));
        self.assertTrue(expt.checkAndAssignUnitCell(filtered_peaks, 1.5, 1.0))  # boolean return value

if __name__ == "__main__":
    unittest.main()
