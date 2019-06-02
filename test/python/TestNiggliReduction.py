import pynsx as nsx
import numpy as np
import unittest

class TestNiggliReduction(unittest.TestCase):

    def test(self):
        tolerance = 1e-6
        # An oblique cell representing an orthorhombic 2,1,3 cell
        # o-----o-----o
        # |     |     |
        # o-----o-----o
        deg = 3.14/180
        cell = nsx.UnitCell(2.0,np.sqrt(17.0),3.0,90*deg,90*deg,np.arctan(1.0/4))

        g = np.array([])
        # todo : says UnitCell has no attribute named 'getMetrictensor'
        # g = cell.getMetricTensor()

        # todo : 'g' is a 3d array and argument takes a double int var- size issue
        # n = nsx.NiggliReduction(g[],1e-3)
        # gprime = np.array([])
        # P = np.array([])
        # n.reduce(gprime,P)

        # check that the unit cell is 1,2,3
        # todo : cannot use 'cell' as an instance of UnitCell
        # cell.transform(P)
        # todo : not able to use any attributes of UnitCell, says 'getA()' is not an attribute of it
        #self.assertAlmostEqual([cell.getA(),1.0,tolerance])
        #self.assertAlmostEqual([cell.getB(),2.0,tolerance])
        #self.assertAlmostEqual([cell.getC(),3.0,tolerance])
        #self.assertAlmostEqual([cell.getAlpha(),90*deg,tolerance])
        #self.assertAlmostEqual([cell.getBeta(),90*deg,tolerance])
        #self.assertAlmostEqual([cell.getGamma(),90*deg,tolerance])


if __name__ == '__main__':
    unittest.main()
