#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestBlob3D.py
##! @brief     Unit tests for class Blob3D
##!
##! @homepage  https://openhkl.org
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import pyohkl as ohkl
import numpy as np
import math
import unittest

class TestBlob3D(unittest.TestCase):

    def testBlob(self):
        '''
        Construct _one_ ellipsoidal blob, then check whether it is correctly analyzed.
        '''

        blob = ohkl.Blob3D()

        c_x = 12.0
        c_y = 15.0
        c_z = 22.0
        sx2 = 2.0
        sy2 = 3.0
        sz2 = 5.0
        M_PI = math.pi
        prefactor = (1.0/pow(2.0*M_PI,1.5))/np.sqrt(sx2*sy2*sz2)
        tot =0.0

        for i in range(0,50):
            for j in range(0,50):
                for k in range(0,50):
                    mass = prefactor*np.exp(-0.5*(pow(i-c_x,2)/sx2+pow(j-c_y,2)/sy2+pow(k-c_z,2)/sz2))
                    blob.addPoint(i,j,k,mass)
                    tot = (tot + mass)

        self.assertAlmostEqual(tot,1.0)
        self.assertAlmostEqual(tot,blob.getMass())

        center = np.zeros(shape=(3,1), dtype=float)
        eigVal = np.zeros(shape=(3,1), dtype=float)
        eigVec = np.zeros(shape=(3,3), dtype=float)

        scale = 1.0

        blob.toEllipsoid(scale,center,eigVal,eigVec)

        # check if center is ok
        self.assertAlmostEqual(center[0,0],c_x)
        self.assertAlmostEqual(center[1,0],c_y)
        self.assertAlmostEqual(center[2,0],c_z)

        # check if the semi_axes are ok
        self.assertAlmostEqual(eigVal[0,0],np.sqrt(sx2))
        self.assertAlmostEqual(eigVal[1,0],np.sqrt(sy2))
        self.assertAlmostEqual(eigVal[2,0],np.sqrt(sz2))

        # check the eigenvectors
        self.assertAlmostEqual(abs(eigVec[0,0]),1.0)
        self.assertAlmostEqual(abs(eigVec[1,1]),1.0)
        self.assertAlmostEqual(abs(eigVec[2,2]),1.0)


if __name__ == '__main__':
    unittest.main()
