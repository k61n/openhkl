import pynsx as nsx
import numpy as np
import ctypes as c
import unittest
import math


class TestMergedPeak(unittest.TestCase):
    
    def test(self):
        deg = math.pi/180.0
        a = 46.3559
        b = 59.9255
        c = 85.5735
        
        alpha = 90.0*deg
        beta = 90.0*deg
        gamma = 90.0*deg

        gruber_tolerance = 1e-2
        
        # spaceGroup group (" P 21 21 21")
        group = nsx.SpaceGroup("P 21 21 21")
        cell = nsx.UnitCell(a, b,c,alpha,beta,gamma)
        G = np.array([(0,0,0),(0,0,0),(0,0,0)])
        G = cell.getMetricTensor()
        P = np.array([(0,0,0),(0,0,0),(0,0,0)])

        gruber =  nsx.GruberReduction(G,gruber_tolerance)

        # todo: nsx has no attribute named "BravaisType"
        # bravais_type = nsx.BravaisType()
        # todo: nsx has no attribute named "LatticeCentring"
        # centering = nsx.LatticeCentring()

        # centering is not defined 
        # match = gruber.reduce(P, centering, bravais_type)
        
        # bravais_type and centering are not defined
        # cell.setBravaisType(bravais_type)
        # cell.setLatticeCentring(centering)

        # todo : type mismatch between numpy and eigen objects
        # cell.transform(P)

        pass



if __name__ == '__main__':
    unittest.main()

  
