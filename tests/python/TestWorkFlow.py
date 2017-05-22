import pynsx as nsx
import numpy as np
import unittest

class TestHDF5(unittest.TestCase):
    
    def test(self):
        expt = nsx.Experiment('test', 'BioDiff2500')
        diff = expt.getDiffractometer()
        data = nsx.DataReaderFactory().create("hdf", "gal3.hdf", diff)
        expt.addData(data)
        source = diff.getSource()

        reader = nsx.HDF5DataReader("gal3.hdf", diff)
        data = nsx.DataSet(reader, diff)

        kernel = nsx.AnnularKernel(data.getNRows(), data.getNCols())
        kernel_image = kernel.getKernel()
        convolver = nsx.Convolver()
        convolver.setKernel(kernel_image)

        finder = nsx.PeakFinder()
        finder.setConvolver(convolver)
        finder.setMinComponents(30)
        finder.setMaxComponents(10000)
        finder.setConfidence(0.98)
        finder.setThresholdType(1)
        finder.setThresholdValue(15.0)
        finder.setKernel(kernel)

        numors = nsx.vector_data()
        numors.push_back(data)
        finder.find(numors)

        peaks = data.getPeaks()

        selected_peaks = []

        for peak in peaks:
            if peak.isSelected() and not peak.isMasked():
                selected_peaks.append(peak)

        self.assertTrue(len(peaks) > 800)
        self.assertTrue(len(selected_peaks) > 650)
               
        indexer = nsx.AutoIndexer(expt, nsx.ProgressHandler())

        for peak in selected_peaks:
            indexer.addPeak(peak)
            peak.getQ()

        params = nsx.IndexerParameters()
        handler = nsx.ProgressHandler()
        indexer.autoIndex(params)

        soln = indexer.getSolutions()[0]

        self.assertTrue(soln[1] > 92.0)

        uc = nsx.UnitCell(soln[0])

        for peak in peaks:
            peak.addUnitCell(uc, True)


if __name__ == '__main__':
    unittest.main()
