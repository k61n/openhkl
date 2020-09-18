#!/usr/bin/env python3

'''
test_data.py

Script for checking data values
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

found_peaks = expt.get_peak_collection("peaks")
peak_list = found_peaks.getPeakList()
for peak in peak_list:
    frame = peak.shape().center()[2]
    print(frame)
