import pynsx as nsx
import numpy as np
import unittest
import sys

class TestWorkFlow(unittest.TestCase):
    
    def test(self):
        expt = nsx.Experiment('test', 'BioDiff2500')
        diff = expt.diffractometer()
        data = nsx.DataReaderFactory().create("hdf", "gal3.hdf", diff)
        expt.addData(data)
        source = diff.getSource()

        reader = nsx.HDF5DataReader("gal3.hdf", diff)
        data = nsx.DataSet(reader)

        finder = nsx.PeakFinder()
        finder.setMinSize(30)
        finder.setMaxSize(10000)
        finder.setMaxFrames(10)
        #finder.setSearchConfidence(0.98)
        #finder.setIntegrationConfidence(0.997)
        finder.setConvolver("annular",{})
        finder.setThreshold(15.0)

        numors = nsx.DataList()
        numors.push_back(data)
        peaks = finder.find(numors)

        selected_peaks = []

        for peak in peaks:
            if peak.isSelected():
                selected_peaks.append(peak)

        self.assertTrue(len(peaks) > 800)
        self.assertTrue(len(selected_peaks) > 650)
               
        indexer = nsx.AutoIndexer(nsx.ProgressHandler())

        for peak in selected_peaks:
            
            d = 1.0 / np.linalg.norm(peak.q().rowVector())

            if (d < 2.0):
                continue

            indexer.addPeak(peak)
            peak.q()

        params = nsx.IndexerParameters()
        handler = nsx.ProgressHandler()
        indexer.autoIndex(params)

        soln = indexer.getSolutions()[0]

        print(soln[0])
        print(soln[1])

        self.assertTrue(soln[1] > 92.0)

        uc = nsx.UnitCell(soln[0])
        sample = diff.getSample()
        sample.addUnitCell(uc)

        for peak in peaks:
            peak.addUnitCell(uc, True)
            peak.q()

        num_peaks = len(peaks)

        return

        #todo: fix up the library test

        library = nsx.ShapeLibrary()

        library_size = 0

        for peak in peaks:
            if not peak.isSelected():
                continue
            if library.addPeak(peak):
                library_size +=1

        print(library_size)

        library.setDefaultShape(library.meanShape())
        predictor = nsx.PeakPredictor(uc, library, 2.1, 50.0, 0)
       
        predicted_peaks = predictor.predict(data)
        print(len(predicted_peaks))
        #self.assertTrue(len(predicted_peaks) > 1600)


if __name__ == '__main__':
    unittest.main()
