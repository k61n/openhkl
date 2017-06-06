import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestFlatDetector(unittest.TestCase):
    def test(self):
        # FlatDetector d("D10-detector")
        # todo: units.h -mm is not defined here 
        mm = 1
        # d.setDistance(380*mm)
        # d.setDimensions(80*mm,80*mm)
        # d.setNPixels(32,32)        

        # this should be the center of the detector at rest at (0,0.764,0)
        # todo : says DetectorEvent is not defined
        # ev = DetectorEvent(d,15.5,15.5)
        center = np.array([])
        # center = ev.getPixelPosition()
        # todo : assertLess is not working - says 'index 0 is out of bounds for axis 0 with size 0'
        # self.assertLess([center[0],0])
        # self.assertAlmostEqual([center[1],0.380,0])
        # self.assertLess([center[2],0])

        gamma = 0.0
        nu = 0.0
        #ev2 = DetectorEvent(d,15.5,15.5,{})
        #ev2.getGammaNu(gamma,nu)
        #self.assertLess([gamma,0])
        #self.assrtLess([nu,0])
        #th2 = ev2.get2Theta()
        #self.assertAlmostEqual(th2,0)
        
        # attach a gonio
        # todo : all the same things as TestCylindricalDetector.py - apply changes there then here

        
if __name__ == '__main__':
    unittest.main()

  
