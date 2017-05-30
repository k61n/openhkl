import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestHDF5IO(unittest.TestCase):
    def test(self):
        factory = nsx.DataReaderFactory()
        ds = nsx.DiffractometerStore()
        # todo : can't convert std::shared_ptr<Diffractrometer>diff
        # todo : can't convert std::shared_ptr<DataSet>dataf

        frames = np.array([]) 
        



if __name__ == '__main__':
    unittest.main()

  
