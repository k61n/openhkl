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
        pixel_position = d.pixelPosition(15.5,15.5)
        center = pixel_position.vector()

        self.assertLess(center[0][0],tolerance)
        self.assertAlmostEqual(center[1],0.380,0)
        self.assertLess(center[2][0],tolerance)

        state1 = nsx.InstrumentState()
        state1.wavelength = 1.0;
        state1.ni = np.array([[0.0,1.0,0.0]],dtype=np.float)
        state1.detectorOffset = np.zeros((3,1),dtype=np.float)
        state1.detectorOrientation = np.identity(3, dtype=np.float)
        state1.sampleOrientation = np.identity(3, dtype=np.float)
        state1.samplePosition = np.zeros((3,1),dtype=np.float)

        gamma = state1.gamma(pixel_position)
        self.assertLess(gamma,tolerance)

        nu = state1.nu(pixel_position)
        self.assertLess(nu,tolerance)
        
        th2 = state1.twoTheta(pixel_position)
        self.assertAlmostEqual(th2,0)
        
        # put detetctor at 90deg, event should point along x
        state2 = nsx.InstrumentState()
        state2.wavelength = 1.0;
        state2.ni = np.array([[0.0,1.0,0.0]],dtype=np.float)
        state2.detectorOffset = np.zeros((3,1),dtype=np.float)
        state2.detectorOrientation = np.array([[0,1,0],[-1,0,0],[0,0,1]], dtype=np.float)
        state2.sampleOrientation = np.identity(3, dtype=np.float)
        state2.samplePosition = np.zeros((3,1),dtype=np.float)

        gamma = state2.gamma(pixel_position)
        self.assertAlmostEqual(gamma,np.deg2rad(90),delta=tolerance)

        nu = state2.nu(pixel_position)
        self.assertLess(nu,0.001,0)
        
        th2 = state2.twoTheta(pixel_position)
        self.assertAlmostEqual(th2,np.deg2rad(90),0)
        
        kf = state2.kfLab(pixel_position).rowVector()
        self.assertAlmostEqual(kf[0,0],1.0, delta=tolerance)
        self.assertLess(kf[0,1],tolerance)
        self.assertLess(kf[0,2],tolerance)

        q = state2.sampleQ(pixel_position).rowVector()

        # should be 45 deg in the x, -y plane
        self.assertAlmostEqual(q[0,0],1, delta=tolerance)
        self.assertAlmostEqual(q[0,1],-1.0, delta=tolerance)
        self.assertLess(q[0,2],tolerance)
        
if __name__ == '__main__':
    unittest.main()

  
