#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestGonio.py
##! @brief     Test ...
##!
##! @homepage  https://openhkl.org
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

# import pyohkl as ohkl
import numpy as np
import unittest

class TestGonio(unittest.TestCase):
    def test(self):
        # a simple translation table
        # todo : Gonio is not defined
        # t = Gonio("Translation table")
        # t.addTranslation("x",np.array([1,0,0]))
        #t.addTranslation("y",np.array([0,1,0]))
        #t.addTranslation("z",np.array([0,0,1]))

        #t.getAxis("x").setPhysical(True)
        #t.getAxis("y").setPhysical(True)
        #t.getAxis("z").setPhysical(True)

        # transform (0,0,0)
        # result = t.Transform(np.array([0,0,0]),{1,2,3})
        # self.assertAlmostEqual(result[0],1,0)
        # self.assertAlmostEqual(result[1],3,0)

        # Check that throws if the number of parameters is invalid

        # todo : no conversion of BOOST_CHECK_THROW
        # BOOST_CHECK_THROW(t.transform(Vector3d(0,0,0),{1,2}),std::range_error);
        #a0 = t.getAxis(0)
        #a1 = t.getAxis(1)
        #a2 = t.getAxis(2)
        ar1 = np.array([])
        #self.assertAlmostEqual(a0.getLabel(),"x")
        #self.assertAlmostEqual(a1.getLabel(),"y")
        #self.assertAlmostEqual(a2.getLabel(),"z")

        # simple goniometer as in Busing Levy
        # todo: Gonio is not defined
        # Gonio g("Busing level convention")
        # todo : whats the conversion of 'RotAxis::CW'
        # g.addRotation("omega",np.array([0,0,1]))
        # g.addRotation("chi",np.array([0,1,0],RotAxis::CW))
        # g.addRotation("phi",np.array([0,0,1],RotAxis::CW))

        # check that result of combined rotation is ok
        deg = 3.14/180.0
        om = 12*deg
        chi = 24*deg
        phi = 55*deg
        OM = np.array([(np.cos(om),np.sin(om),0),(-np.sin(om),np.cos(om),0),(0,0,1)])
        CH = np.array([(np.cos(chi),0,np.sin(chi)),(0,1,0),(-np.sin(chi),0,np.cos(chi))])
        PH = np.array([(np.cos(phi),np.sin(chi),0),(-np.sin(chi),np.cos(chi),0),(0,0,1)])

if __name__ == '__main__':
    unittest.main()
