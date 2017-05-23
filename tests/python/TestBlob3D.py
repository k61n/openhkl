import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestBlob3D(unittest.TestCase):
       
    def TestBlob(self):
        
        blob = nsx.Blob3D()
        
        c_x = 12.0
        c_y = 15.0
        c_z = 22.0
        sx2 = 2.0 
        sy2 = 3.0
        sz2 = 5.0
        M_PI = 3.14
        prefactor = (1.0/pow(2.0*M_PI,1.5))/np.sqrt(sx2*sy2*sz2)
        tot =0.0
        mass = 1
        # todo: import value of "mass" unit in py
        for i in range(0,50):
            for j in range(0,50):
                for k in range(0,50):
                    mass = prefactor*np.exp(-0.5*(pow(i-c_x,2)/sx2+pow(j-c_y,2)/sy2+pow(k-c_z,2)/sz2))
                    blob.addPoint(i,j,k,mass)
                    tot = (tot + mass)    

        # todo: test failed while running assertalmostequal, says its not defined
        self.assertAlmostEqual(tot,1.0)
        self.assertAlmostEqual(tot,blob.getMass())
        
        center = np.array([])
        eigVal = np.array([])
        eigVec = np.array([])

        sigma1 = 0.682689492
    
        # todo: error in 'toEllipsoid' argument type- not imported from c++ to py 
        blob.toEllipsoid(sigma1,center,eigVal,eigVec)

        #todo: check if center is ok AND also test failed while running asserAlms..
        self.assertAlmostEqual(center(0),c_x)
        self.assertAlmostEqual(center(1),c_y)
        self.assertAlmostEqual(center(2),c_z)

        # todo: check if the semi_axes are ok AND same issue of assertAlm...
        self.assertAlmostEqual(eigVal(0),sqrt(sx2))
        self.assertAlmostEqual(eigVal(1),sqrt(sy2))
        self.assertAlmostEqual(eigVal(2),sqrt(sz2))
    
        # todo: check the eigenvectors AND same issue of assertAlm...
        self.assertAlmostEqual(((eigVec.col(0))(0)),1.0)
        self.assertAlmostEqual(((eigVec.col(1))(1)),1.0)
        self.assertAlmostEqual(((eigVec.col(2))(2)),1.0)
        


if __name__ == '__main__':
    unittest.main()

