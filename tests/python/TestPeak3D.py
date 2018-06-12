import pynsx as nsx
import math
import numpy as np
import ctypes as c
import unittest

tolerance = 1e-6

# todo: fix this test!!

class TestPeak3D(unittest.TestCase):

    # todo: fix Gonio::addRotation()   
    def test(self):
        d = nsx.FlatDetector("D10-detector")
        mm = 1.0e-3 
        d.setDistance(380*mm)
        d.setHeight(80*mm)
        d.setWidth(80*mm)
        d.setNCols(32)        
        d.setNRows(32)        

        #event = nsx.DetectorEvent(d,15.5, 15.5, 0.0, [])

        source = nsx.Source()
        mono = nsx.Monochromator("mono")
        source.addMonochromator(mono)
        source.setSelectedMonochromator(0)

        #peak = nsx.Peak3D()    
        #peak.setSource(source)
        #peak.setDetectorEvent(event)

        #Q = peak.getQ()

        #self.assertAlmostEqual(Q[0,0], 0)
        #self.assertAlmostEqual(Q[0,1], 0)
        #self.assertAlmostEqual(Q[0,2], 0)
        
        #g = nsx.Gonio("Gamma")
    
        #g.addRotation("Gamma",np.array([0,0,1], dtype=float), nsx.RotAxis.CW)        
        # todo (Jonathan) : in method 'Component_setGonio', argument 2 of type 'nsx::sptrGonio'
        #d.setGonio(g)
        
        #deg = math.pi/180.0
        # todo: fix the remainder of this test to accommodate updated DetectorEvent interface
        #event2 = nsx.DetectorEvent(d,15.5,15.5, 0.0, [90.0*deg])
        
        #peak.setDetectorEvent(event2)
        #Q = peak.getQ()
        #self.assertAlmostEqual(Q[0,0], 1)
        #self.assertAlmostEqual(Q[0,1], -1)
        #self.assertLess(Q[0,2],tolerance)    

        sample = nsx.Sample("sample")
        b1 = nsx.Gonio("Busing level convention")
        
        #todo: complete test...




if __name__ == '__main__':
    unittest.main()

  
