import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestHDF5IO(unittest.TestCase):
    def test(self):
        factory = nsx.DataReaderFactory()
        diff = nsx.Diffractometer.build("D10")
        dataf = factory.create("","D10_ascii_example",diff)
        dataf.open()
        
        frames = []
        i = 0
        for i in range(0,dataf.nFrames()): 
            frames.append(dataf.frame(i))
        
        dataf.saveHDF5("D10_hdf5_example.h5")
        dataf.close()
            
        # read data back in and check that it agrees!
        dataf = factory.create("h5","D10_hdf5_example.h5", diff)
        
        for j in range(0,dataf.nFrames()):
            self.assertTrue((dataf.frame(j) == frames[j]).all())
       
        dataf.close()


if __name__ == '__main__':
    unittest.main()

  
