import pynsx as nsx
import numpy as np
import ctypes as c
import unittest

tolerance = 1e-5

class TestBasis(unittest.TestCase):

    def test_basis(self):
        bprime = nsx.Basis(np.array([2,0,0],dtype=float),np.array([0,2,0], dtype =float), np.array([0,0,1], dtype = float))
        # todo : dont know how to correct the following line
        # bsecond = (np.array([1,1,0]),np.array([-1,1,0]),np.array([0,0,1]),bprime)
        x = np.array([1,0,0])
        
        #todo: check xsecond  = bsecond.fromStandard(x)
        # bsecond not defined
        # xsecond  = bsecond.fromStandard(x)
        
        # 1
        #self.assertAlmostEqual(xsecond[0],0.25,tolerance)
        #self.assertAlmostEqual([xsecond[1],-0.25,tolerance])
        #self.assertLess(xsecond[2],tolerance)

        # 2
        x = bsecond.toStandard(xsecond)
        #self.assertAlmostEqual([x[0],1.0,tolerance])
        #self.assertLess(x[1],tolerance)
        #self.assertLess(x[2],tolerance)

        # 3
        # todo : check if the declarations of xr and xrsecond are correct
        xr = np.array([1,0,0])
        xrsecond = bsecond.fromReciprocalStandard(xr)
        #self.assertAlmostEqual([xrsecond[0],2.0,tolerance])
        #self.assertAlmostEqual([xrsecond[1],-2.0,tolerance])
        #self.assertLess(xrsecond[2],tolerance)

        # 4
        # todo: check if the declaration of xr is correct, maybe its not getting compiled
        xr = bsecond.toReciprocalStandard(xrsecond)
        #self.assertAlmostEqual([xr[0],1.0])
        #self.assertLess(xr[1],tolerance)
        #self.assertLess(xr[2],tolerance)
        
        #5
        # todo :check x definition
        bsecond.rebaseToStandard();
        x = bsecond.toStandard(xsecond)
        self.assertAlmostEqual(xrsecond(0),2.0)
        self.assertAlmostEqual([xrsecond(1),-2.0])
        self.assertAlmostEqual([xrsecond(2),0])

        #6
        x =bsecond.toStandard(xsecond)
        self.assertAlmostEqual([x(0),1.0])
        self.assertAlmostEqual([x(1),0])
        self.assertAlmostEqual([x(2),0])

        #7
        xrsecond = bsecond.fromReciprocalStandard(xr)
        self.assertAlmostEqual([xrsecond(0),2.0])
        self.assertAlmostEqual([xrsecond(1),-2.0])
        self.assertAlmostEqual([xrsecond(2),0])

        #8
        xr = bsecond.toReciprocalStandard(xrsecond)
        self.assertAlmostEqual([xr(0),1.0])
        self.assertAlmostEqual([xr(1),0])
        self.assertAlmostEqual([xr(2),0])

        #9
        # todo : check the rebasing to the first basis
        # todo : check if the bsecond.rebaseTo(bprime) is correct 
        bsecond.rebaseTo(bprime)
        xsecond = bsecond.fromStandard(x)
        self.assertAlmostEqual([xsecond(0),0.25])
        self.assertAlmostEqual([xsecond(1),-0.25])
        self.assertAlmostEqual([xsecond(2),0])

        #10
        x =bsecond.toStandard(xsecond)
        self.assertAlmostEqual([x(0),1.0])
        self.assertAlmostEqual([x(1),0]) 
        self.assertAlmostEqual([x(2),0])

        #11
        xrsecond = bsecond.fromReciprocalStandard(xr)
        self.assertAlmostEqual([xrsecond(0),2.0])
        self.assertAlmostEqual([xrsecond(0),-2.0])
        self.assertAlmostEqual([xrsecond(2),0])

        #12
        xr = bsecond.toReciprocalStandard(xrsecond)
        self.assertAlmostEqual([xr(0),1.0])
        self.assertAlmostEqual([xr(1),0])
        self.assertAlmostEqual([xr(2),0])

        reference = nsx.Basis(np.array([1,0,0]),np.array([0,1,0], np.array([0,0,1])))
        
        P = np.array([])

        # todo : check sigma and error propagations
        # todo : how to import !reference-->hasSigmas()
        # self.assertAlmostEqual()

        self.assertAlmostEqual([err_a,0.01])
        self.assertAlmostEqual([err_b,0.01])
        self.assertAlmostEqual([err_c,0.01])
        self.assertAlmostEqual([err_alpha,0])
        self.assertAlmostEqual([err_beta,0])
        self.assertAlmostEqual([err_gamma,0])

        # todo: don't know how to import  P << 0,2,0,4,0,0,0,0,-1;
        P = np.array([0,2,0,4,0,0,0,0,-1])
        reference.transform(P)

        reference.getParametersSigmas(err_a,err_b,err_c,err_alpha,err_beta,err_gamma)
        
        reference.getReciprocalParametersSigmas(err_as,err_bs,err_cs,err_alphas,err_betas, err_gammas)
        

if __name__ == '__main__':
    unittest.main()

  
