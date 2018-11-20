import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestHDF5IO(unittest.TestCase):
    
    def test(self):
    
        experiment = nsx.Experiment("","D10")
        dataset = nsx.DataSet("","D10_ascii_example",experiment.diffractometer())
        dataset.open()
        
        frames = []
        i = 0
        for i in range(0,dataset.nFrames()): 
            frames.append(dataset.frame(i))
        
        dataset.saveHDF5("D10_hdf5_example.h5")
        dataset.close()
            
        # read data back in and check that it agrees!
        dataset = nsx.DataSet("h5","D10_hdf5_example.h5", experiment.diffractometer())
        
        for j in range(0,dataset.nFrames()):
            self.assertTrue((dataset.frame(j) == frames[j]).all())
       
        dataset.close()


if __name__ == '__main__':
    unittest.main()

  
