import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

tolerance = 1e-6

# todo: fix this test to accommodate the updated DetectorEvent interface

class TestFlatDetector(unittest.TestCase):
    def test(self):
        d = nsx.FlatDetector("D10-detector")
        mm = 1e-3
        d.setDistance(380*mm)
        d.setDimensions(80*mm,80*mm)
        d.setNPixels(32,32)        

        # this should be the center of the detector at rest at (0,0.764,0)
        #ev = nsx.DetectorEvent(d,15.5,15.5, 0.0, [])
        #center = ev.getPixelPosition()

        #self.assertLess(center[0][0],tolerance)
        #self.assertAlmostEqual(center[1],0.380,0)
        #self.assertLess(center[2][0],tolerance)

        #gamma = nsx.new_double()
        #nu = nsx.new_double()
 
        #ev2 = nsx.DetectorEvent(d,15.5,15.5, 0.0, [])
        #ev2.getGammaNu(gamma,nu)
        #self.assertLess(nsx.get_value(gamma),tolerance)
        #self.assertLess(nsx.get_value(nu),tolerance)
        #th2 = ev2.get2Theta()
        #self.assertAlmostEqual(th2,0)
        
        # attach a gonio
        # todo : all the same things as TestCylindricalDetector.py - apply changes there then here

        
if __name__ == '__main__':
    unittest.main()

  
