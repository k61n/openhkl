#!/usr/bin/env python3

'''
plotdetector.py

Load data from a NSX HDF5 file, plot the peaks in the detector coordinate system
'''

import argparse
import os.path
from nsx.experiment import Experiment, pynsxprint
from nsx.parameters import Parameters
from nsx.peakplot import PeakPlot
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool plotting script')
parser.add_argument('-n', '--name', type=str, dest='name',
                    help='name of system', required=True)
parser.add_argument('-d', '--detector', type=str, dest='detector',
                    help='Type of detector', required=True)
parser.add_argument('-p', '--parameters', type=str, dest='paramfile',
                    default='parameters', help='File containing experiment paramters')
parser.add_argument('--predicted', action='store_true', dest='predicted', default=False,
                    help='Saved data in .nsx has completed prediction step')

args = parser.parse_args()

params = Parameters()
if os.path.isfile(args.paramfile):
    pynsxprint(f"Reading parameters from {args.paramfile}")
    params.load(args.paramfile)
else:
    raise RuntimeError("No parameters file detected")

expt = Experiment(args.name, args.detector, params)
expt.set_logger()
logger = expt.get_logger()
expt.load(predicted=args.predicted)
det = expt.get_detector()
plotter = PeakPlot(args.name, det.minRow(), det.maxRow(), det.minCol(), det.maxCol())
collections = expt.get_collection_names()
for name in collections:
    plotter.add_peak_collection(expt.get_peak_collection(name), name)
plotter.plot()
plotter.save()

