##  ***********************************************************************************************
##
##  NSXTool: data reduction for neutron single-crystal diffraction
##
##! @file      test/python/TestSpaceGroupCSV.py
##! @brief     Test ...
##!
##! @homepage  ###HOMEPAGE###
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import pynsx as nsx
# import numpy as np
import unittest

class TestSpaceGroupCSV(unittest.TestCase):

    def test(self):
        num_rows = 0

        counts = map
        symbols = nsx.SpaceGroup.symbols()
        for symbol in symbols:
            # syntax error
            #counts([symbol]) = 0
            pass


if __name__ == '__main__':
    unittest.main()
