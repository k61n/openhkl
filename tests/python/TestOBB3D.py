import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

class TestOBB3D(unittest.TestCase):
    
    def test(self):
        tolerance = 1e-5
        tolerance_large = 1.0
        # test the construction of an OBB
        center = np.array([3,4,7], dtype=float) 
        extent = np.array([np.sqrt(2),np.sqrt(2),1])   
        axis = np.array([(1,0,0),(0,1,0),(0,0,1)], dtype=float)
        # nsx has no atribute named-V -R  OBB
        obb1 = nsx.OBB(center,extent,axis)
        lower = np.array([])
        lower = obb1.getLower()
        upper = np.array([])
        upper = obb1.getUpper()

        var = np.sqrt(2)
        x = 3.0 - var

        self.assertAlmostEqual(lower[0], x)
        self.assertAlmostEqual(lower[1], 4.0-var, delta = tolerance)
        self.assertAlmostEqual(lower[2],6.0,delta = tolerance)
        self.assertAlmostEqual(upper[0],3.0+var,delta = tolerance)
        self.assertAlmostEqual(upper[1],4.0+var,delta = tolerance)
        self.assertAlmostEqual(upper[2],8.0,delta = tolerance)

        # test- the rotation of an OBB
        r = np.array([(1,-1,0),(1,1,0),(0,0,1)],dtype=float)
        
        # obb1.rotate(r) doesnt work
        obb1.rotate(r)
        lower = obb1.getLower()
        upper = obb1.getUpper()
        
        self.assertAlmostEqual(lower[0],1.0,delta = tolerance)
        self.assertAlmostEqual(lower[1],2.0,delta = tolerance)
        self.assertAlmostEqual(lower[2],6.0,delta = tolerance)
        self.assertAlmostEqual(upper[0],5.0,delta = tolerance)
        self.assertAlmostEqual(upper[1],6.0,delta = tolerance)
        self.assertAlmostEqual(upper[2],8.0,delta =tolerance)


        # test- the isotropic scaling of an OBB
        center = np.array([3,2,4],dtype =float)
        extent = np.array([1,4,2],dtype = float)
        axis = np.array([(1,0,0),(0,1,0),(0,0,1)],dtype=float)
        
        # weird - declaring a a new instance of OBB is giving error
        obb2 = nsx.OBB(center,extent,axis)
        obb2.scale(5)
        lower = obb2.getLower()
        upper = obb2.getUpper()
        self.assertAlmostEqual(lower[0,0],-2.0,delta = tolerance)
        self.assertAlmostEqual(lower[1],-18.0,delta = tolerance)
        self.assertAlmostEqual(lower[2],-6.0,delta = tolerance)
        self.assertAlmostEqual(upper[0],8.0,delta = tolerance)
        self.assertAlmostEqual(upper[1],22.0,delta = tolerance)
        self.assertAlmostEqual(upper[2],14.0,delta = tolerance)

        # test- the anisotropic scaling of an OBB
        center = np.array([2,2,1],dtype=float)
        extent = np.array([np.sqrt(2)/2, np.sqrt(2), 2],dtype=float)
        axis = np.array([(1,-1,0),(1,1,0),(0,0,1)],dtype=float) 

        
        obb3 = nsx.OBB(center,extent,axis)
        obb3.scale(np.array((3,2,5))
        # todo: lower = obb3.getLower() - syntax error
        #lower = obb3.getLower()
        #upper = obb3.getUpper()
        #self.assertAlmostEqual([lower[0],-1.5,tolerance])
        #self.assertAlmostEqual([lower[1],-1.5,tolerance])
        #self.assertAlmostEqual([lower[2],-9.0,tolerance])
        #self.assertAlmostEqual([upper[0],5.5,tolerance])
        #self.assertAlmostEqual([upper[1],5.5,tolerance])
        #self.assertAlmostEqual([upper[2],11.0,tolerance])

        # test the translation of an OBB
        # translate function doesnt work
        # obb3.translate(np.array([-1,2,4]))
        #lower = obb3.getLower()
        #upper = obb3.getUpper()
        #self.assertAlmostEqual([lower[0],-2.5,tolerance])
        #self.assertAlmostEqual([lower[1],0.5,tolerance])
        #self.assertAlmostEqual([lower[2],-5.0,tolerance])
        #self.assertAlmostEqual([upper[0],4.5,tolerance])
        #self.assertAlmostEqual([upper[1],7.5,tolerance])
        #self.assertAlmostEqual([upper[2],15.0,tolerance])

        # todo : test a given point falls inside the OBB
        #obb3.translate(np.array([1,-2,-4]))
        lower = obb3.getLower()
        upper = obb3.getUpper()

        #elf.assertAlmostEqual([lower[0],-1.5,tolerance])
        #self.assertAlmostEqual([lower[1],-1.5,tolerance])
        #self.assertAlmostEqual([lower[2],-9.0,tolerance])
        #self.assertAlmostEqual([upper[0],5.5,tolerance])
        #self.assertAlmostEqual([upper[1],5.5,tolerance])
        #self.assertAlmostEqual([upper[2],11.0,tolerance])

        # todo : error in declaring of int variable without initial value
        # nStepps_int = None
        # delta = np.array([upper-lower]/nSteps])
        # point = np.array([0,0,0,1])
        # (wrong)sum = 0 ,dtype= double
        i=0
        nSteps = 500
        for i in range(nSteps+1):
            pass
        #    point.x() = lower[0]+i*delta[0]
        #    for j=0 in range(0,nSteps+1)
        #        point.y() = lower[1] + j*delta[1]
        #        for k=0 in range(0,nSteps+1)
        #            point.z() = lower[2]+k*delta[2]
        #            if obb3.isInside(point)
        #                sum = sum +1.0
        #            k=k+1
        #        j = j+1
        #    i = i+1
        
        #sum = sum*(((upper[0]-lower[0])*(upper[1]-lower[1])*(upper[2]-lower[2]))/(nSteps*nSteps*nSteps))
        #self.assertAlmostEqual([sum, 480, tolerance_large])

if __name__ == '__main__':
    unittest.main()

  
