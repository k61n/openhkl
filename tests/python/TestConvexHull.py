import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestComplexHull(unittest.TestCase):
    def test(self):
        # ! check that the Hull satisfies the convexity condition. This consists in
        # ! checking that the signed volume between every face and every point is positive.
        # ! This shows that each point is inside every face and therefore the hull is convex.

        def checkConvexity(chull): # todo : check if it is correctly imported    
            #todo : getFaces() is not imported to py 
            faces = chull.getFaces()
            vertices = chull.getVertices()

            # todo : cannot use faces instance
           # for f in faces
            #    for v in vertices
             #       if(f.volumeSign(v) < 0)
              #          return False
               # return True
    
    def Test_ConvexHull():
        CHullDouble = ConvexHall(dtype = double)
        # to be done ...


if __name__ == '__main__':
    unittest.main()

  
