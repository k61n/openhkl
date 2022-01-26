#!/usr/bin/python
##  ***********************************************************************************************
##
##  NSXTool: data reduction for neutron single-crystal diffraction
##
##! @file      scripts/qspace-ellipsoid.py
##! @brief     Transform peaks to q-space ellipsoids to examine shapes
##!
##! @homepage  ###HOMEPAGE###
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import sys
import numpy as np
import math
import unittest
from pathlib import Path
import argparse
import matplotlib.pyplot as plt
from pdb import set_trace

lib_dir = "@SWIG_INSTALL_PATH@" # Path to pynsx.py
sys.path.append(lib_dir)
import pynsx as nsx

parser = argparse.ArgumentParser(description="Reciprocal space shape test")
parser.add_argument('file', help='.nsx file name')
parser.add_argument('-n', '--name', type=str, dest='name', help='Name of system',
                    required=True)
parser.add_argument('-d', '--diffractometer', type=str, dest='diffractometer',
                    help='Name of diffractometer', required=True)
parser.add_argument('-p', '--peakcollection', type=str, dest='peakcollection',
                    help='Name of peak collection', required=True)
parser.add_argument('--nx', type=int, dest='nx', default=2,
                    help='Frame subdivisions in x direction')
parser.add_argument('--ny', type=int, dest='ny', default=2,
                    help='Frame subdivisions in y direction')
parser.add_argument('--nz', type=int, dest='nz', default=1,
                    help='Frame subdivisions in z direction')
args = parser.parse_args()

def angle_between(v1, v2):
    angle = np.arccos(np.abs(np.dot(v1, v2)))
    return (angle / 2.0 / np.pi) * 360.0

def major_axis(shape):
    eigenvalues, eigenvectors = np.linalg.eigh(shape)
    imax = np.argmax(eigenvalues)
    return eigenvectors[:,imax]

def segment_data(data, peaks, nx, ny, nz):
    ncols = data.nCols()
    nrows = data.nRows()
    nframes = data.nFrames()

    segmented_data = {}

    window = (math.floor(ncols / nx), math.floor(nrows / ny), math.floor(nframes / nz))
    for x in range(nx):
        x_min = x * window[0]
        x_max = x_min + window[0]
        for y in range(ny):
            y_min = y * window[1]
            y_max = y_min + window[1]
            for z in range(nz):
                z_min = z * window[2]
                z_max = z_min + window[2]

                shapes = []
                for peak in peaks.getPeakList():
                    if (not peak.enabled()):
                        continue
                    c = peak.shape().center()
                    if (c[0] > x_min and c[0] < x_max and
                        c[1] > y_min and c[1] < y_max and
                        c[2] > z_min and c[2] < z_max):
                        shapes.append(peak.qShape().metric())
                segmented_data[(x, y, z)] = shapes

    return segmented_data

data_file = args.file

expt = nsx.Experiment(args.name, args.diffractometer)
expt.loadFromFile(data_file)
peaks = expt.getPeakCollection(args.peakcollection)
data = expt.getAllData()[0]


segmented_data = segment_data(data, peaks, args.nx, args.ny, args.nz)

for k in range(args.nz):
    fig, axs = plt.subplots(args.nx, args.ny)
    for i in range(args.nx):
        for j in range(args.ny):
            shapes = segmented_data[(i, j, k)]
            angles = []
            ax0 = None
            for shape in shapes:
                axis = major_axis(shape)
                if (not isinstance(ax0, np.ndarray)):
                    ax0 = axis
                else:
                    angles.append(angle_between(ax0, axis))
            axs[i, j].hist(angles, bins=90)

    plt.show()
