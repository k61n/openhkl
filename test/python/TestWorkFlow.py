#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestWorkFlow.py
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

class TestWorkFlow(unittest.TestCase):

    def test(self):
        expt = ohkl.Experiment('test', 'BioDiff2500')
        diff = expt.diffractometer()
        dataset = ohkl.DataSet("TestWorkFlow.py", diff)
        dataset.addDataFile("gal3.hdf", "ohkl");
        dataset.finishRead()
        expt.addData(dataset)
        source = diff.source()

        finder = ohkl.PeakFinder()
        finder.setMinSize(30)
        finder.setMaxSize(10000)
        finder.setMaxFrames(10)

        convolver = ohkl.AnnularConvolver({})

        finder.setConvolver(convolver)
        finder.setThreshold(15.0)

        numors = ohkl.DataList()
        numors.push_back(dataset)
        peaks = finder.find(numors)

        selected_peaks = []

        for peak in peaks:
            if peak.enabled():
                selected_peaks.push_back(peak)

        self.assertTrue(len(peaks) > 800)
        self.assertTrue(len(selected_peaks) > 650)

        indexer = ohkl.AutoIndexer(ohkl.ProgressHandler())

        for peak in selected_peaks:

            d = 1.0 / np.linalg.norm(peak.q().rowVector())

            if (d < 2.0):
                continue

            indexer.addPeak(peak)
            peak.q()

        params = ohkl.IndexerParameters()
        handler = ohkl.ProgressHandler()
        indexer.autoIndex(params)

        soln = indexer.solutions()[0]

        self.assertTrue(soln[1] > 92.0)

        uc = ohkl.UnitCell(soln[0])

        for peak in peaks:
            peak.setUnitCell(uc)
            peak.q()

        num_peaks = len(peaks)

        return

        #todo: fix up the library test

        library = ohkl.ShapeModel()

        library_size = 0

        for peak in peaks:
            if not peak.enabled():
                continue
            if library.addPeak(peak):
                library_size +=1

if __name__ == '__main__':
    pass
    # unittest.main()
