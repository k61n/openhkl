import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestEllipsoid3d(unittest.TestCase):
    def test(self):
        center = np.array([10,10,10])
        semi_axes = np.array([3,3,4])
        eigV = np.array([(1,0,0),(0,1,0),(0,0,1)])
        
        # todo: Ellipsoid is not defined
        #e = Ellipsoid(center,semi_axes,eigV)
        #e.translate(np.array([1,0,0]))



if __name__ == '__main__':
    unittest.main()

  
