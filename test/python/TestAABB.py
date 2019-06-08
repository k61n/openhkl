import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestAABB3D(unittest.TestCase):

   
    def testBounds(self):
        lb = np.array([0, 0, 0], dtype=float)
        ub = np.array([1, 2, 3], dtype=float)
        bb = nsx.AABB()
        bb.setLower(lb)
        bb.setUpper(ub)
        center = bb.center()
        extents = bb.extents()
        
        self.assertAlmostEqual(center[0], 0.5)
        self.assertAlmostEqual(center[1], 1.0)
        self.assertAlmostEqual(center[2], 1.5)
        
        self.assertAlmostEqual(extents[0], 1)
        self.assertAlmostEqual(extents[1], 2)
        self.assertAlmostEqual(extents[2], 3)

    def testConstructor(self):
        lb = np.array([-1, -2, -3], dtype=float)
        ub = np.array([1, 2, 3], dtype=float)
        bb = nsx.AABB(lb, ub)

        lb = bb.lower();
        ub = bb.upper();

        self.assertAlmostEqual(lb[0], -1)
        self.assertAlmostEqual(lb[1], -2)
        self.assertAlmostEqual(lb[2], -3)

        self.assertAlmostEqual(ub[0], 1)
        self.assertAlmostEqual(ub[1], 2)
        self.assertAlmostEqual(ub[2], 3)

if __name__ == '__main__':
    unittest.main()

  
