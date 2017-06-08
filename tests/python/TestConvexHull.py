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
   
        # creates an empty convex HUll
        chull = nsx.ConvexHull()
        
        # todo : check if this below line is actually is analog of its c++ counterpart
        # Checks that the hull can not be updated with 0point to be processed
        # todo: check exception throwing
        #self.assertRaises(Exception, chull.updateHull())
        
        
        # todo : check if its working - "chull.addVertex(vec1)", c++ code has "std::runtime_error"
        # todo : what is the analog of std::runtime_error to python
        # Fill it with three vertices to form a tetrahedron
        vec1 = np.array([0,0,0], dtype=float)
        chull.addVertex(vec1)
        # Checks that the hull can not be updated with only 1 point to be processed
        # todo : what is conversion of BOOST_CHECK_THROW and NO_THROW
        #self.assertAlmostEqual(chull.updateHull(),0)
                    
        # Fill it with three vertices to form a tetrahedron
        vec2 =  np.array([10,0,0], dtype=float)
        chull.addVertex(vec2)
        #Checks that the hull can not be updated with only 2 point to be processed
        #self.assertAlmostEqual([chull.updateHull(),0])

        vec3 = np.array([0,10,0], dtype=float)
        chull.addVertex(vec3)
        #Checks that the hull can not be updated with only 3 point to be processed
        #self.assertAlmostEqual([0,0,10])

        vec4 = np.array([0,0,10], dtype=float)
        chull.addVertex(vec4)
        # Checks that with 4 vertices the hull can be built
        chull.updateHull()

        faces = chull.getFaces()
        edges = chull.getEdges()
        vertices = chull.getVertices()

        #  todo : not working - BOOST_CHECK_EQUAL(vertices.size(),4);
        # todo: update python bindings (AttributeError: 'SwigPyObject' object has no attribute 'size')
        self.assertAlmostEqual(vertices.size(),4)
        self.assertAlmostEqual(edges.size(),6)
        self.assertAlmostEqual(faces.size(),4)
        
        # ! Checks that the hull satisfies the Euler conditions
        # todo : check if assertAlmostEqual is right conversion here
        self.assertAlmostEqual(checkEulerConditions())

        # ! Checks that the hull satisfies the Convexity condition
        self.assertAlmostEqual(checkConvexity(chull))
        
        # Fill the convex hull with new vertices in order to make a cube
        chull.addVertex(np.array([10,10,0]))
        chull.addVertex(np.array([10,0,10]))
        chull.addVertex(np.array([0,10,10]))
        chull.addVertex(np.array([10,10,10]))

        # update the hull
        chull.updateHull()

        # Check that the number of vertices, edges and faces corresponds to a cube
        self.assertAlmostEqual(vertices.size(),8)
        self.assertAlmostEqual(edges.size(),18)
        self.assertAlmostEqual(faces.size(),12)
        
        # ! Checks that the hull satisfies the Euler conditions
        self.assertAlmostEqual(checkEulerConditions())

        # ! Checks that the hull satisfies the Convexity condition
        self.assertAlmostEqual(checkConvexity(chull))
        
        # ! Checks that the volume of the cube is 10*10*10=1000
        self.assertAlmostEqual([chull.getVolume(),1000,0])

        oldVolume = chull.getVolume() 
        chull.translateToCenter()
        newVolume = chull.getVolume()
        self.assertAlmostEqual([oldVolume,newVolume,0])

        # Check that the copy construction is OK
        newHull(CHullDouble, chull) 
        self.assertAlmostEqual([chull.getNVertices(),newHull.getNVertices()],0)
        self.assertAlmostEqual([chull.getNEdges(),newHull.getNEdges()],0)
        self.assertAlmostEqual([chull.getNFaces(),newHull.getNFaces()],0)
        self.assertAlmostEqual([chull.getNVolume(),newHull.getNVolume()],0)


if __name__ == '__main__':
    unittest.main()

  
