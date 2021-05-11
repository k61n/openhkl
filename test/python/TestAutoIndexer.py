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

        expt.loadFromFile("FutA-2.nsx")
        found_peaks = expt.getPeakCollection("peaks")
        n_peaks = found_peaks.numberOfPeaks()
        self.assertEqual(n_peaks, 269)

        # Filter the peaks
        filter = expt.peakFilter()
        filter.setFilterStrength(True)
        filter.setFilterDRange(True)
        filter.setDRange(1.55, 50.0)
        filter.setStrength(1.0, 1.0e7)
        filter.filter(found_peaks)
        expt.acceptFilter("filtered", found_peaks)
        filtered_peaks = expt.getPeakCollection("filtered")
        n_caught = filtered_peaks.numberCaughtByFilter()
        self.assertEqual(n_caught, 160)
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
        autoindexer.setParameters(autoindexer_params)
        autoindexer.autoIndex(filtered_peaks)
        # reference_cell = expt.getReferenceCell()
        # accepted_cell = expt.getAcceptedCell()
        # print("Reference cell = " + reference_cell.toString())
        # print(autoindexer.solutionsToString())
        self.assertTrue(expt.checkAndAssignUnitCell(filtered_peaks, 1.0, 1.0))  # boolean return value

if __name__ == "__main__":
    unittest.main()
