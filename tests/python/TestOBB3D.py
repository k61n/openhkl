import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestOBB3D(unittest.TestCase):
    tolerance = 1e-5
    tolerance_large = 1.0
    def test(self):
        # todo : test the construction of an OBB
        center = np.array([3,4,7]) 
        extent = np.array([np.sqrt(2),np.sqrt(2),1])   
        axis = np.array([(1,0,0),(0,1,0),(0,0,1)])
        # todo : nsx has no atribute named OBB
        obb1 = nsx.OBB(center,extent,axis)
    


if __name__ == '__main__':
    unittest.main()

  
