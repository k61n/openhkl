##  ***********************************************************************************************
##
##  NSXTool: data reduction for neutron single-crystal diffraction
##
##! @file      test/python/TestPeakFinder.py
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

class TestPeakFinder(unittest.TestCase):
    def test(self):

        factory = nsx.DataReaderFactory()

        peakFinder = nsx.PeakFinder()
        ph = nsx.ProgressHandler()

        #peakFinder.setIntegrationConfidence(0.997)
        #peakFinder.setSearchConfidence(0.97)

        peakFinder.setMaxSize(10000)
        self.assertTrue(peakFinder.maxSize() == 10000)

        peakFinder.setMinSize(10)
        self.assertTrue(peakFinder.minSize() == 10)

        peakFinder.setMaxFrames(10)
        self.assertTrue(peakFinder.maxFrames() == 10)

if __name__ == '__main__':
    unittest.main()
