#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestSpaceGroupCSV.py
##! @brief     Test ...
##!
##! @homepage  https://openhkl.org
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import pyohkl as ohkl
# import numpy as np
import unittest

class TestSpaceGroupCSV(unittest.TestCase):

    def test(self):
        num_rows = 0

        counts = map
        symbols = ohkl.SpaceGroup.symbols()
        for symbol in symbols:
            # syntax error
            #counts([symbol]) = 0
            pass


if __name__ == '__main__':
    unittest.main()
