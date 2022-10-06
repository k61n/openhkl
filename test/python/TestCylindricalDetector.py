#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestCylindricalDetector.py
##! @brief     Test ...
##!
##! @homepage  https://openhkl.org
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import pyohkl as ohkl
import numpy as np
import unittest

tolerance = 1e-3

# todo: fix this test to accomodate the new DetectorEvent interface

class TestCylindricalDetector(unittest.TestCase):

    def test(self):
        d = ohkl.CylindricalDetector("D19-detector")
        mm = 1e-3
        d.setDistance(764*mm)
        d.setAngularWidth(np.deg2rad(120.0))
        cm = 1e-2
        d.setHeight(40.0*cm)
        d.setNCols(640)
        d.setNRows(256)

        pixel_position = d.pixelPosition(319.5, 127.5)
        center = pixel_position.vector()

        self.assertLess(center[0,0],tolerance)
        self.assertAlmostEqual(center[1,0],0.764)
        self.assertLess(center[2,0],tolerance)

        state1 = ohkl.InstrumentState()

        # should be center of the detector so gamma, nu =0 at rest

        gamma = state1.gamma(pixel_position)
        self.assertLess(gamma,tolerance)

        nu = state1.nu(pixel_position)
        self.assertLess(nu,tolerance)

        th2 = state1.twoTheta(pixel_position)
        self.assertAlmostEqual(th2,0)

        # put detetctor at 90deg, event should point along x
        state2 = ohkl.InstrumentState()
        state2.detectorOrientation = np.array([[0,1,0],[-1,0,0],[0,0,1]], dtype=np.float)

        gamma = state2.gamma(pixel_position)
        self.assertAlmostEqual(gamma,np.deg2rad(90),delta=tolerance)

        nu = state2.nu(pixel_position)
        self.assertLess(nu,0.001,0)

        th2 = state2.twoTheta(pixel_position)
        self.assertAlmostEqual(th2,np.deg2rad(90),0)

        kf = state2.kfLab(pixel_position).rowVector()
        self.assertAlmostEqual(kf[0,0],1.0, delta=tolerance)
        self.assertLess(kf[0,1],tolerance)
        self.assertLess(kf[0,2],tolerance)

        q = state2.sampleQ(pixel_position).rowVector()

        # should be 45 deg in the x, -y plane
        self.assertAlmostEqual(q[0,0],1, delta=tolerance)
        self.assertAlmostEqual(q[0,1],-1.0, delta=tolerance)
        self.assertLess(q[0,2],tolerance)


if __name__ == '__main__':
    unittest.main()
