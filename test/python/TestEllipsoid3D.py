##  ***********************************************************************************************
##
##  NSXTool: data reduction for neutron single-crystal diffraction
##
##! @file      test/python/TestEllipsoid3D.py
##! @brief     Test ...
##!
##! @homepage  ###HOMEPAGE###
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

# import pynsx as nsx
import numpy as np
import unittest

class TestEllipsoid3d(unittest.TestCase):
    def test(self):
        center = np.array([10,10,10])
        semi_axes = np.array([3,3,4])
        eigV = np.array([(1,0,0),(0,1,0),(0,0,1)])

        # todo: Ellipsoid is not defined
        #e = Ellipsoid(center,semi_axes,eigV)
        #e.translate(np.array([1,0,0]))

if __name__ == '__main__':
    unittest.main()
