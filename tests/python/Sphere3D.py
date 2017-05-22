import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestAABB3D(unittest.TestCase):

   
    def testBounds(self):
        lb = np.array([0, 0, 0], dtype=float)
        ub = np.array([1, 2, 3], dtype=float)
        bb = nsx.AABB3D()
        bb.setBounds(lb, ub)
        center = bb.getAABBCenter()
        extents = bb.getAABBExtents()
        
        self.assertAlmostEqual(center[0], 0.5)
        self.assertAlmostEqual(center[1], 1.0)
        self.assertAlmostEqual(center[2], 1.5)
        
        self.assertAlmostEqual(extents[0], 1)
        self.assertAlmostEqual(extents[1], 2)
        self.assertAlmostEqual(extents[2], 3)

    def testConstructor(self):
        lb = np.array([-1, -2, -3], dtype=float)
        ub = np.array([1, 2, 3], dtype=float)
        bb = nsx.AABB3D(lb, ub)

        lb = bb.getLower();
        ub = bb.getUpper();

        self.assertAlmostEqual(lb[0], -1)
        self.assertAlmostEqual(lb[1], -2)
        self.assertAlmostEqual(lb[2], -3)

        self.assertAlmostEqual(ub[0], 1)
        self.assertAlmostEqual(ub[1], 2)
        self.assertAlmostEqual(ub[2], 3)

    def testVolume(self):
        lb = np.array([-1, -2, -3], dtype=float)
        ub = np.array([1, 2, 3], dtype=float)
        bb = nsx.AABB3D(lb, ub)

        self.assertAlmostEqual(bb.AABBVolume(), 48)

    def testRayInterset(self):
        t1 = nsx.new_double()
        t2 = nsx.new_double()
        lb = np.array([1,1,1], dtype=float)
        ub = np.array([3,3,3], dtype=float)
        bb = nsx.AABB3D(lb, ub)

        start = np.array([-1, -1, -1], dtype=float)
        end = np.array([1,1,1], dtype=float)
        
        collide = bb.rayIntersect(start, end, t1, t2)

        self.assertTrue(collide)

        t1 = nsx.get_value(t1)
        t2 = nsx.get_value(t2)

        self.assertAlmostEqual(t1, 2)
        self.assertAlmostEqual(t2, 4)

        

if __name__ == '__main__':
    unittest.main()

  
