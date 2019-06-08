import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestUnitCell(unittest.TestCase):
    a = 6.32
    b = 7.22
    c = 3.44
    # todo: deg is not exported in python
    deg = np.pi / 180.0
    alpha = 90*deg
    parameters = [a,b,c,alpha]

    def setUp(self):
        a,b,c,alpha = self.parameters
        self.cell = nsx.UnitCell(a,b,c,alpha,alpha,alpha)

    def testParameters(self):
        cell = self.cell
        ch = cell.character()
        
        a,b,c,alpha = self.parameters
        
        self.assertAlmostEqual(ch.a,a)
        self.assertAlmostEqual(ch.b,b)
        self.assertAlmostEqual(ch.c,c)
        
        self.assertAlmostEqual(ch.alpha,alpha)
        self.assertAlmostEqual(ch.beta,alpha)
        self.assertAlmostEqual(ch.gamma,alpha)
        

if __name__ == '__main__':
    unittest.main()

  
