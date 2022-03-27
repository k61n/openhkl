import sys
import glob
import unittest
import pynsx as nsx

class TestAutoIndexer(unittest.TestCase):

    def test(self):
        # Initialise the experiment
        name = "CrChiA"
        detector = "BioDiff2500"
        expt = nsx.Experiment(name, detector)

        expt.loadFromFile("CrChiA.nsx")
        found_peaks = expt.getPeakCollection("found")
        n_peaks = found_peaks.numberOfPeaks()
        self.assertEqual(n_peaks, 492)

        # Filter the peaks
        filter = expt.peakFilter()
        filter.flags().strength = True
        filter.flags().d_range = True
        filter.parameters().d_min = 1.55
        filter.parameters().d_max = 50.0
        filter.parameters().strength_min = 1.0
        filter.parameters().strength_max = 1.0e7
        filter.filter(found_peaks)
        name = expt.generatePeakCollectionName()
        expt.acceptFilter(name, found_peaks)
        filtered_peaks = expt.getPeakCollection(name)
        n_caught = filtered_peaks.numberCaughtByFilter()
        self.assertEqual(n_caught, 391)
        print(f'Autoindex: {n_caught}/{n_peaks} peaks caught by filter')

        # Autoindex the peaks
        autoindexer = expt.autoIndexer()
        expt.setReferenceCell(57.96, 65.12, 86.52, 90.0, 90.0, 90.0)
        autoindexer_params = nsx.IndexerParameters()
        autoindexer_params.maxdim = 100.0
        autoindexer_params.nSolutions = 10
        autoindexer_params.nVertices = 10000
        autoindexer_params.subdiv = 30
        autoindexer_params.indexingTolerance = 0.2
        autoindexer_params.minUnitCellVolume = 10000.0
        autoindexer.autoIndex(filtered_peaks)
        reference_cell = expt.getReferenceCell()
        reference_cell.setSpaceGroup(nsx.SpaceGroup("P 21 21 21"))
        self.assertTrue(expt.checkAndAssignUnitCell(filtered_peaks, 2.0, 0.1))  # boolean return value

if __name__ == "__main__":
    unittest.main()
