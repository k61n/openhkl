#!/usr/bin/env python3

'''
test_filter.py

Test the peak filter for NSXTool
'''

import argparse
import os.path
from nsx.experiment import Experiment, pynsxprint
from nsx.parameters import Parameters
from nsx.peakplot import PeakPlot
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool unit cell/instrument parameters refinement script')
parser.add_argument('-n', '--name', type=str, dest='name', help='name of system', required=True)
parser.add_argument('-d', '--detector', type=str, dest='detector',
                    help='Type of detector', required=True)
parser.add_argument('-p', '--parameters', type=str, dest='paramfile',
                    default='parameters', help='File containing experiment paramters')
parser.add_argument('--predicted', action='store_true', dest='predicted',
                    help='Saved data in .nsx has completed prediction step')
parser.add_argument('-b', '--batches', type=int, dest='batches', default=10,
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
expt.load(predicted=args.predicted)
expt.set_space_group()
found_peaks = expt.get_peak_collection('peaks')

npeaks = found_peaks.numberOfPeaks()
params.filter['extinct'] = True
ncaught = expt.filter_peaks(params.filter, found_peaks, 'filtered')
pynsxprint("Filter caught " + str(ncaught) + " of " + str(npeaks) + " peaks")
expt.refine(args.batches)
npeaks = found_peaks.numberOfPeaks()
ncaught = expt.filter_peaks(params.filter, found_peaks, 'filtered')
pynsxprint("Filter caught " + str(ncaught) + " of " + str(npeaks) + " peaks")
