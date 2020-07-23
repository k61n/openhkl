#!/usr/bin/env python3

'''
peakplot.py

Plot peaks in the coordinates of the detector image (uniits of pixels)
'''

import numpy as np
import matplotlib.pyplot as plt
from pdb import set_trace

class PeakPlot:

    def __init__(self, name, x_min, x_max, y_min, y_max):
        self._name = name
        plt.xlim(x_min, x_max)
        plt.ylim(y_min, y_max)
        self._peaks = {}

    def add_peak(self, peak, datakey):
        coords = peak.shape().center()
        if datakey in self._peaks:
            self._peaks[datakey].push_back((coords[0], coords[1], coords[2]))
        else:
            self._peaks[datakey] = [(coords[0], coords[1], coords[2]),]

    def add_peak_collection(self, collection, name):
        for peak in collection.getPeakList():
            self.add_peak(peak, name)

    def plot(self):
        for datakey in self._peaks:
            data = np.array(self._peaks[datakey])
            plt.plot(data[:,1,0], data[:,0,0], 'ko', markersize=2, label=datakey)

    def save(self):
        fname = self._name + ".pdf"
        plt.show()
        plt.savefig(fname, bbox_inches="tight")

