#!/usr/bin/env python3

'''
autoindex.py

This script computes the unit cell from raw diffraction data in the form of a
number of .tiff images. Note that it save intermediate data to a file 
<name>.nsx after the integration step, and if this file is found, it will load
it in subsequent runs. Note that this depends on the same .tiff files being
loaded.
'''

import sys
import argparse
import os.path
from nsx.experiment import Experiment, pynsxprint
from nsx.parameters import Parameters
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool autoindexing test script')
parser.add_argument('--name', type=str, dest='name', help='name of system')
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='.tiff raw data files')
parser.add_argument('--detector', type=str, dest='detector', default='BioDiff5000',
                    help='Type of detector')
parser.add_argument('--loadnsx', action='store_true', dest='loadnsx', default=False,
                    help='load <name>.nsx')
parser.add_argument('-p', '--parameters', type=str, dest='paramfile',
                    default='parameters', help='File containing experiment paramters')
parser.add_argument('--length_tol', type=float, dest='length_tol',
                    default=1.0, help='length tolerance (a, b, c) for autoindexing')
parser.add_argument('--angle_tol', type=float, dest='angle_tol',
                    default=5.0, help='angle tolerance (alpha, beta, gamma) for autoindexing')
args = parser.parse_args()

params = Parameters()
if os.path.isfile(args.paramfile):
    pynsxprint(f"Reading parameters from {args.paramfile}")
    params.load(args.paramfile)
else:
    raise RuntimeError("No parameters file detected")

expt = Experiment(args.name, args.detector, params)
if args.loadnsx:
    if os.path.isfile(expt.nsxfile):
        pynsxprint(f"{expt.nsxfile} exists, loading experiment")
        expt.load()
    else:
        raise OSError("f{expt.nsxfile} not found")
else:
    filenames = args.files
    dataset = "all"
    pynsxprint("Loading data...")
    expt.add_data_set(dataset, filenames)
    pynsxprint("...data loaded\n")
    pynsxprint("Finding peaks...")
    expt.find_peaks([expt.get_data(dataset)])
    pynsxprint("...peak finding complete\n")
    pynsxprint("Integrating...")
    npeaks = expt.integrate_peaks()
    pynsxprint("...integration complete\n")
    pynsxprint(f"Saving experiment to file {expt.nsxfile}")
    expt.save()

pynsxprint("Filtering...")
ncaught = expt.filter_peaks(params.filter)
pynsxprint("...filtering complete\n")
pynsxprint("Filter caught " + str(ncaught) + " of " + str(npeaks) + " peaks")

pynsxprint("Autoindexing...")
expt.autoindex(args.length_tol, args.angle_tol)
pynsxprint("...autoindexing complete\n")
expt.print_unit_cells()
