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
import unittest
from pathlib import Path
import argparse
import matplotlib.pyplot as plt

parser = argparse.ArgumentParser(description="Reciprocal space shape test")
parser.add_argument('file', help='.nsx file name')
parser.add_argument('-n', '--name', type=str, dest='name', help='Name of system',
                    required=True)
parser.add_argument('-d', '--diffractometer', type=str, dest='diffractometer',
                    help='Name of diffractometer', required=True)
parser.add_argument('-p', '--peakcollection', type=str, dest='peakcollection',
                    help='Name of peak collection', required=True)
args = parser.parse_args()

def angle_between(v1, v2):
    angle = np.arccos(np.dot(v1, v2))
    return (angle / 2.0 / np.pi) * 360.0

lib_dir = "@SWIG_INSTALL_PATH@" # Path to pynsx.py
data_file = args.file

sys.path.append(lib_dir)
import pynsx as nsx

# set up the experiment
expt = nsx.Experiment(args.name, args.diffractometer)
expt.loadFromFile(data_file)
peaks = expt.getPeakCollection(args.peakcollection)

shapes = []
for peak in peaks.getPeakList():
    if (peak.enabled()):
        shapes.append(peak.qShape().metric())

axes = []
for shape in shapes:
    eigenvalues, eigenvectors = np.linalg.eigh(shape)
    i = np.argmax(eigenvalues)
    major_axis = eigenvectors[:,i]
    axes.append(major_axis)

angles = []
for vector in axes[1:]:
    angles.append(angle_between(axes[0], vector))

fig, ax = plt.subplots()
ax.hist(angles, bins=180)
plt.show()
