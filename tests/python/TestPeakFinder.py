import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestPeakFinder(unittest.TestCase):
    def test(self):
        # todo : nsx.DataList() doesnt work- says it doesnt exist in nsx
        # numors = nsx.DataList()
        factory = nsx.DataReaderFactory()
        ds = nsx.DiffractometerStoreBase_Instance()
        # todo : the fuction below doesnt exist in nsx
        # diff = nsx.sptrDiffractrometer(ds.buildDiffractromerer("D10"))
        # todo : error in using 'factory.create("","D10_ascii_example",diff)'
        # dataf = factory.create("","D10_ascii_example",diff)
        # todo : eroor in using 'dataf.getMetadata()'
        # meta = dataf.getMetadata()
        peakFinder = nsx.PeakFinder()
        ph = nsx.ProgressHandler()
        # todo : error in using 'nsx.sptrProgressHandler'
        # handler = nsx.sptrProgressHandler(ph)    

        # error in "meta" is not defined
        # error in most of lines below is that they are not defined i.e the lines of its declaration has error and are hence in are in comment
        # self.assertTrue(meta.getKey("nbang") == 2)
        # dataf.open()
        # numors.push_back(dataf)
        # peakFinder.setHandler(handler)

        peakFinder.setConfidence(0.997)        
        # todo : error in "second" argument of the function
        # self.assertAlmostEqual([(peakFinder.confidence()),0.997,1e-10])
        
        peakFinder.setMaxComponents(10000)
        self.assertTrue(peakFinder.getMaxComponents() == 10000)

        peakFinder.setMinComponents(10)
        self.assertTrue(peakFinder.getMinComponents() == 10)

        peakFinder.setThresholdValue(3.0)
        self.assertAlmostEqual(peakFinder.getThresholdValue(), 3.0 , 1e-10) 
        
        # error since 'numors' is not defined.
        #result = peakFinder.find(numors)
        #self.assertTrue(result == True)

        # todo : dont know how to convert 'size_t'
        #size_t  num_peaks = dataf.getPeaks().size()

        # todo : check the whole code snippet below. (4 lines)
        # self.assertTrue(num_peaks == 1)
        # dataf.close()
        # dataf.clearPeaks()
        # dataf = Nullptr

        

if __name__ == '__main__':
    unittest.main()

  
