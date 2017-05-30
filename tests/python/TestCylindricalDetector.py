import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestCylindricalDetector(unittest.TestCase):
    def test(self):
        d = nsx.CylindricalDetector("D19-detector") 
        mm = 1.0
        # todo : convert mm from unit.h
        d.setDistance(764*mm)
        # todo : define 'deg'
        deg = 3.14/180.0
        d.setAngularWidth(120.0*deg)
        cm = 1.0
        d.setHeight(40.0*cm)
        d.setNPixels(640,256)

        # todo : DetectorEvent is not defined
        # ev1 =DetectorEvent(d,319.5,127.5)
        # This should be the center of the detector at rest at (0,0.764,0)
        # vec3d = np.array([])
        # center = ev1.getPixelPosition(vec3d)

        # self.assertLess([center[0],0])
        # self.assertAlmostEqual([center[0],0.764,0])
        # self.assertLess([center[2],0])

        # shouldd be center of the detector so gamma, nu =0 at rest
        gamma = 0.0
        nu = 0.0
        # ev1.getGammaNu(gamma,nu)
        # self.assertLess([gamma,0])        
        # self.assertLess([nu,0])
        # th2 = ev1.get2Theta()
        # self.assertLess([th2,0])

        # attach a gonio
        # todo: problem in importing 'std::shared_ptr<Gonio> g(new Gonio("gamma-arm"))'
        # g = new Gonio("gamma-arm")      
        # todo : problem in importing "RotAxis::CW"
        # g.addRotation("gamma",np.array([0,0,1], RotAxis::CW)
        # d.setGonio(g)

        # put detetctor at 90deg, event should point along x
        # ev2 = DetectorEvent(d,319.5,127.5,{90.0*deg}) 
        # center = ev2.getPixelPosition()
        # self.assertAlmostEqual([center[0],0.764,0])
        # self.assertLess([center[1],0.001])
        # self.assrtLess([center[2],0.001])
        # ev2.getGammaNu(gamma,nu)
        # self.assertAlmostEqual([gamma,90*deg,10e-3])
        # self.assertLess(nu,0.001,0)

        # th2 = ev2.get2Theta()
        # self.assertAlmostEqual(th2, 90*deg,0)

        # Scattering in the center of the detector with wavelength 2.0
        # should get kf = (0.5,0,0)
        kf = np.array([])
        # kf = ev2.getKf(2.0)
        # self.assertAlmostEqual(kf[0],0.5,0)
        # self.assertLess([kf[1],0.001])
        # self.assertLess([kf[2],0.001])

        # should be 45 deg in the x, -y plane
        # self.assertAlmostEqual([Q[0],0.5,0])
        # self.assertAlmostEqual([Q[1],-0.5,0])
        # self.assertLess([Q[2],0.001])

        # Check that detector receive an scattering vector at the right position
        # basically defininig three double type variables
        px = 0.0 
        py = 0.0 
        t = 0.0 

        # from = np.array([0,0,0])
        # d.receiveKf(px,py,kf,from,t, {90.0*deg})
        # self.assertAlmostEqual([px,319.5,0])
        # self.assertAlmostEqual([py,127.5,0])
        
        # todo : if its actually 0.0*deg, which doesnt make sense , could have written 0
        # d.receiveKf(px,py,np.array([0,0,0.764,0.10],from, t, {0.0*deg}))
        # self.assertAlmostEqual([px,319.5,0])
        # self.assertAlmostEqual([py,191.25,0])

        # d.receiveKf(px,py, np.array([1,0,0],from, t, {90.0*deg}))
        # self.assertAlmostEqual(px, 319.5,0)
        # self.assertAlmostEqual(py,127.5,0)


if __name__ == '__main__':
    unittest.main()

  
