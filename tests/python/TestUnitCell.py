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
        a,b,c,alpha = self.parameters

        # todo: UnitCell.getA(), etc. not exported to python
        # self.assertAlmostEqual(cell.getA(),a)
        #self.assertAlmostEqual(cell.getB(),b)
        #self.assertAlmostEqual(cell.getC(),c);
        # todo: UnitCell.getAlpha() etc. not exported to python
        #self.assertAlmostEqual(cell.getAlpha(),alpha);
        #self.assertAlmostEqual(cell.getBeta(),alpha);
        #self.assertAlmostEqual(cell.getGamma(),alpha);
        

if __name__ == '__main__':
    unittest.main()

  
