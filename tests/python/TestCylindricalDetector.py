import pynsx as nsx
import numpy as np
import ctypes as c
import math
import unittest

tolerance = 1e-3

class TestCylindricalDetector(unittest.TestCase):
    def test(self):
        d = nsx.CylindricalDetector("D19-detector") 
        mm = 1e-3
        d.setDistance(764*mm)
        deg = math.pi/180.0
        d.setAngularWidth(120.0*deg)
        cm = 1e-2
        d.setHeight(40.0*cm)
        d.setNPixels(640,256)

        ev1 = nsx.DetectorEvent(d,319.5,127.5)
        # This should be the center of the detector at rest at (0,0.764,0)
        #vec3d = np.array([0,0,0], dtype=float)
        center = ev1.getPixelPosition()

        self.assertLess(center[0,0],tolerance)
        self.assertAlmostEqual(center[1,0],0.764)
        self.assertLess(center[2,0],tolerance)

        # should be center of the detector so gamma, nu =0 at rest
         gamma = nsx.new_double()
        nu = nsx.new_double()
        ev1.getGammaNu(gamma,nu)
        self.assertLess(nsx.get_value(gamma),tolerance)        
        self.assertLess(nsx.get_value(nu),tolerance)
        th2 = ev1.get2Theta()   
        self.assertLess(th2,tolerance)

        # attach a gonio
         g = nsx.Gonio("gamma-arm")      
        g.addRotation("gamma",np.array([0,0,1], dtype=float), nsx.RotAxis.CW)
        d.setGonio(g)

        # put detetctor at 90deg, event should point along x
        ev2 = nsx.DetectorEvent(d,319.5,127.5,[90.0*deg]) 
        center = ev2.getPixelPosition()
        self.assertAlmostEqual(center[0,0],0.764, delta=tolerance)
        self.assertLess(center[1,0],0.001)
        self.assertLess(center[2,0],0.001)
        ev2.getGammaNu(gamma,nu)
        self.assertAlmostEqual(nsx.get_value(gamma),90*deg,delta=tolerance)
        self.assertLess(nsx.get_value(nu),0.001,0)

        th2 = ev2.get2Theta()
        self.assertAlmostEqual(th2, 90*deg,0)

        # Scattering in the center of the detector with wavelength 2.0
        # should get kf = (0.5,0,0)
        kf = ev2.getKf(2.0)
        self.assertAlmostEqual(kf[0,0],0.5, delta=tolerance)
        self.assertLess(kf[1,0],0.001)
        self.assertLess(kf[2,0],0.001)

        # should be 45 deg in the x, -y plane
        Q = ev2.getQ(2.0)
        self.assertAlmostEqual(Q[0,0],0.5, delta=tolerance)
        self.assertAlmostEqual(Q[1,0],-0.5, delta=tolerance)
        self.assertLess(Q[2,0],0.001)

        # Check that detector receive an scattering vector at the right position
        # basically defininig three double type variables
        px = 0.0 
        py = 0.0 
        t = 0.0 

        frm = np.array([0,0,0], dtype=float)
        px = nsx.new_double()
        py = nsx.new_double()
        t = nsx.new_double()
        d.receiveKf(px,py,kf,frm,t, [90.0*deg])
        self.assertAlmostEqual(nsx.get_value(px),319.5)
        self.assertAlmostEqual(nsx.get_value(py),127.5,0)
        
        d.receiveKf(px,py,np.array([0.0,0.764,0.10]),frm, t, [0.0*deg])
        self.assertAlmostEqual(nsx.get_value(px),319.5)
        self.assertAlmostEqual(nsx.get_value(py),191.25)

        d.receiveKf(px,py, np.array([1,0,0], dtype=float),frm, t, [90.0*deg])
        self.assertAlmostEqual(nsx.get_value(px), 319.5, delta=tolerance)
        self.assertAlmostEqual(nsx.get_value(py),127.5, delta=tolerance)


if __name__ == '__main__':
    unittest.main()

