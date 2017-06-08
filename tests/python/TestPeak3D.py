import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestPeak3D(unittest.TestCase):

    # todo: fix Gonio::addRotation()   
    def test(self):
        d = nsx.FlatDetector("D10-detector")
        # todo: export mm to python
        mm = 1.0e-3 
        d.setDistance(380*mm)
        d.setDimensions(80*mm,80*mm)
        d.setNPixels(32,32)        

        event = nsx.DetectorEvent(d,15.5, 15.5)

        source = nsx.Source()
        mono = nsx.Monochromator("mono")
        source.addMonochromator(mono)
        source.setSelectedMonochromator(0)

        peak = nsx.Peak3D()    
        # todo: source object is not callable; 'setDetectorEvent' is not callable
        peak.setSource(source)
        peak.setDetectorEvent(event)

        # todo:peak3D getQ one argument is missing
        Q = peak.getQ()

         # says: Q not defined
        #self.assertAlmostEqual(Q[0], 0)
        #self.assertAlmostEqual(Q[1], 0)
        #self.assertAlmostEqual(Q[2], 0)
        
        g = nsx.Gonio("Gamma")
    
        # todo : g.addRotation("Gamma",np.array([0,0,1], dtype=float), 0)        
        
        # argument error--d.setGonio(g)
        deg = 3.14/180.0
        #says DetectorEvent is not defined 
        #event2 = self.DetectorEvent(d,15.5,15.5,(90.0*deg))
        
        # ^event2 is not defined
        # peak.setDetectorEvent(self,event2)

        #Q = peak.getQ()



if __name__ == '__main__':
    unittest.main()

  
