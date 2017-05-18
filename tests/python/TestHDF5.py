import pynsx as nsx
import numpy as np
import unittest

class TestHDF5(unittest.TestCase):
    
    def test(self):
        #expt = nsx.Experiment("test", "BioDiff2500")
        diff_store = nsx.DiffractometerStore()
        biodiff = diff_store.buildDiffractometer('BioDiff2500')
        data_reader = nsx.HDF5DataReader('gal3.hdf', biodiff)
        #reader = nsx.DataReaderFactory().create("hdf", "../gal3.hdf")
        


if __name__ == '__main__':
    unittest.main()
