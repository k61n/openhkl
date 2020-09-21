#!/usr/bin/env python3

'''
test_autoindexer.py

Test the new autoindexer workflow
'''

import argparse
import os.path
from nsx.experiment import Experiment, pynsxprint
from nsx.parameters import Parameters
from nsx.peakplot import PeakPlot
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool autoindexer test script')
parser.add_argument('-n', type=str, dest='name', help='name of system', required=True)
parser.add_argument('-d', type=str, dest='detector', help='Type of detector', required=True)
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='Data files')
parser.add_argument('--dataformat', type=str, dest='dataformat',
                    help='Format of data files')
parser.add_argument('-p', '--parameters', type=str, dest='paramfile',
                    default='parameters', help='File containing experiment paramters')
parser.add_argument('--loadnsx', action='store_true', dest='loadnsx', default=False,
                    help='load <name>.nsx')
parser.add_argument('--length_tol', type=float, dest='length_tol',
                    default=1.0, help='length tolerance (a, b, c) for autoindexing')
parser.add_argument('--angle_tol', type=float, dest='angle_tol',
                    default=0.5, help='angle tolerance (alpha, beta, gamma) for autoindexing')
parser.add_argument('--frame_min', type=float, dest='frame_min',
                    default=10.0, help='Minimum frame index for autoindexing')
parser.add_argument('--frame_max', type=float, dest='frame_max',
                    default=50.0, help='Maximum frame index for autoindexing')

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
if not args.loadnsx:
    if not args.dataformat:
        raise RuntimeError("Command line argument --dataformat must be specified")
    if not args.files:
        raise RuntimeError("No data files specified (--files)")
    filenames = args.files
    expt.set_data_format(args.dataformat)
    pynsxprint("Loading data...")
    if args.dataformat == 'raw':
        name = 'all'
        expt.add_data_set(name, filenames)
    elif args.dataformat == 'nexus':
        for filename in filenames:
            expt.add_data_set(filename, filename)
    elif args.dataformat == 'hdf5':
        expt.add_data_set(filenames[0], filenames[0])
    pynsxprint("...data loaded\n")
    data = expt.get_data()
    pynsxprint("Finding peaks...")
    expt.find_peaks(data, 0, -1)
    pynsxprint("Integrating...")
    npeaks = expt.integrate_peaks()
    expt.save()
else:
    expt.loadpeaks()

found_peaks = expt.get_found_peaks()
success = expt.run_auto_indexer(found_peaks, args.length_tol, args.angle_tol,
                                args.frame_min, args.frame_max)
if success:
    pynsxprint("Autoindexing successful")
else:
    pynsxprint("Autoindexing failed")
