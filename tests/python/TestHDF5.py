import pynsx as nsx
import numpy as np
import unittest

class TestHDF5(unittest.TestCase):
    
    def test(self):
        biodiff = nsx.Diffractometer.build('BioDiff2500')
        data_reader = nsx.HDF5DataReader('gal3.hdf', biodiff)

if __name__ == '__main__':
    unittest.main()
