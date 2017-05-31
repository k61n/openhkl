import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestNDTRee(unittest.TestCase):
    def test(self):

        #todo : cant import 'using Ellipsoid3D = Ellipsoid<double, 3>'
        tree = np.array([(0,0,0),(100,100,100)]) 
        vects = np.array([])        
        radius =0.45
        vals = np.array([radius,radius,radius])
        center = np.array([0,0,0])

        # todo : not successfully converted-'std::set<const IShape<double, 3>*> test_set'
        # todo : not able to convert "std::vector<Ellipsoid3D*> shapes;"
        
        # Lattice of non-intersecting spheres
        i = 1
        j = 1
        k = 1
        for i in range(1,20):
            for j in range(1,20):
                for k in range(1,20):
                    center = np.array([i,j,k])    
                    # todo : Ellipsoid3D is not defined
                    # shape = Ellipsoid3D(center,vals,vects) 
                    # shape.emplace_back(shape)
                    # tree.addData(shape)                   
                    k=k+1
                j=j+1 
            i = i+1

        # check that the data was inserted correctly
        
        numChambers = 0
        for chamber in tree:
            numChambers = numChambers + 1
            #for shape in chamber. getData():
                # test_set.insert(shape)                          

        # todo: tree.numChambers() dont work
        # self.assertAlmostEqual([numChambers,tree.numChambers()])

        # todo : check why 'test_set.size()' doesn't work
        # self.assertAlmostEqual(test_set.size(),19*19*19)
        
        # todo : tree.getCollsions().size() doesn't work
        # check that they dont intersect
        # self.assertAlmostEqual([tree.getCollisions().size(),0])

        # add some spheres which will intersect
        center = np.array([1.5,1.5,1.5])
        vals = np.array([ radius,radius,radius ])
        # ellipsoid3D is not defined
        # shape = Ellipsoid3D(center, vals, vects)

        # self.assertAlmostEqual(tree.getCollisions(Shape).size(),8)
        pass




if __name__ == '__main__':
    unittest.main()

  
