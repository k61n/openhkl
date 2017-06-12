import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

tolerance = 1e-5

class TestBasis(unittest.TestCase):

    def test(self):
        # todo : " module 'pynsx' has no attribute 'sptrBasis'"
        # bprime = nsx.sptrBasis( nsx.Basis(np.array([2,0,0]),np.array([0,2,0]),np.array([0,0,1])))
        
        # todo : type mismatch between NumPy and Eigen Objects
        #bsecond = nsx.Basis(np.array([1,1,0]),np.array([-1,1,0]),np.array([0,0,1]))
        x = np.array([1,0,0])

        # bsecond is not defined
        # xsecond = bsecond.fromStandard(x)
        
        #1
        #self.assertAlmostEqual(xsecond(0),0.25,tolerance)
        #self.assertAlmostEqual(xsecond(0),-0.25,tolerance)
        #self.assertLess(xsecond(2),tolerance)

        #2
        #x = bsecond.toStandard(xsecond)

        pass


if __name__ == '__main__':
    unittest.main()

  
