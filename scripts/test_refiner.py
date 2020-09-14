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

parser = argparse.ArgumentParser(description='NSXTool unit cell/instrument parameters refinement script')
parser.add_argument('-n', '--name', type=str, dest='name',
                    help='name of system', required=True)
parser.add_argument('-d', '--detector', type=str, dest='detector',
                    help='Type of detector', required=True)
parser.add_argument('-p', '--parameters', type=str, dest='paramfile',
                    default='parameters', help='File containing experiment paramters')
parser.add_argument('--predicted', action='store_true', dest='predicted', default=False,
                    help='Saved data in .nsx has completed prediction step')
parser.add_argument('--frames', type=int, nargs=2, dest='frame_reange', default=[0.10],
                    help='Frames to using for refinement')
parser.add_argument('-b', '--batches', type=int, dest='batches', default=1, required=True,
                    help='Number of batches of frames')

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
expt.load()
collection_name = "peaks"
data = expt.get_data()[0]
refinement_collection = expt.get_peak_collection(collection_name)
cell = expt.get_accepted_cell()
expt.refine(refinement_collection, cell, data, args.batches)
