#!/usr/bin/env python3

'''
autoindex.py

This script computes the unit cell from raw diffraction data in the form of a
number of .tiff images. Note that it save intermediate data to a file
<name>.nsx after the integration step, and if this file is found, it will load
it in subsequent runs. Note that this depends on the same .tiff files being
loaded.
'''

import argparse
import os.path
import logging
from numpy import arange
from nsx.experiment import Experiment, pynsxprint
from nsx.parameters import Parameters
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool autoindexing test script')
parser.add_argument('--name', type=str, dest='name', help='name of system')
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='.tiff raw data files')
parser.add_argument('--detector', type=str, dest='detector', required=True,
                    help='Type of detector')
parser.add_argument('--dataformat', type=str, dest='dataformat',
                    help='Format of data files')
parser.add_argument('--loadnsx', action='store_true', dest='loadnsx', default=False,
                    help='load <name>.nsx')
parser.add_argument('--frames', dest='frames_range', type=int, nargs=2, default=[0,10],
                    help='frame range for autoindexing')
parser.add_argument('-p', '--parameters', type=str, dest='paramfile',
                    default='parameters', help='File containing experiment paramters')
parser.add_argument('--length_tol', type=float, dest='length_tol',
                    default=1.0, help='length tolerance (a, b, c) for autoindexing')
parser.add_argument('--angle_tol', type=float, dest='angle_tol',
                    default=5.0, help='angle tolerance (alpha, beta, gamma) for autoindexing')
parser.add_argument('--verbose', action='store_true', dest='verbose', default=False,
                    help='Print verbose output')
args = parser.parse_args()


def autoindex_test(expt, param, p_min, p_max, step, dtype, length_tol, angle_tol):
    peaks = expt.filtered_collection
    for p in arange(p_min, p_max, step):
        expt.set_parameter(param, dtype(p))
        cell_found = expt.autoindex_peaks(peaks, length_tol, angle_tol)
        if cell_found:
            expt.log(f'Autoindexing successful: {param} = {p}')
            expt.log('Correct cell found: ' + expt.get_accepted_cell().toString())


params = Parameters()
if os.path.isfile(args.paramfile):
    pynsxprint(f"Reading parameters from {args.paramfile}")
    params.load(args.paramfile)
else:
    raise RuntimeError("No parameters file detected")

dataset = "all"
numors = []
expt = Experiment(args.name, args.detector, params, args.verbose)
expt.set_logger()
logger = expt.get_logger()
nsxfile = expt.get_nsxfile()

if args.loadnsx:
    if os.path.isfile(nsxfile):
        pynsxprint(f"{nsxfile} exists, loading experiment")
        expt.load()
    else:
        raise OSError("f{nsxfile} not found")
else:
    if not args.dataformat:
        raise RuntimeError('data file format not specified (--dataformat)')
    expt.set_data_format(args.dataformat)
    pynsxprint("Loading data...")
    if args.dataformat == 'raw':
        name = 'all'
        expt.add_data_set(name, args.files)
        numors.append(expt.get_data(name))
    elif args.dataformat == 'nexus':
        for filename in args.files:
            expt.add_data_set(filename, filename)
            numors.append(expt.get_data(filename))
    pynsxprint("...data loaded\n")
    pynsxprint("Finding peaks...")
    expt.find_peaks(numors, args.frames_range[0], args.frames_range[1])
    pynsxprint("...peak finding complete\n")
    pynsxprint("Integrating...")
    npeaks = expt.integrate_peaks()
    pynsxprint("...integration complete\n")
    pynsxprint(f"Saving experiment to file {nsxfile}")
    pynsxprint("Filtering...")
    ncaught = expt.filter_peaks(params.filter)
    pynsxprint("...filtering complete\n")
    pynsxprint("Filter caught " + str(ncaught) + " of " + str(npeaks) + " peaks")
    expt.save()

pynsxprint("Testing Autoindexer...")
# autoindex_test(expt, "n_solutions", 5, 15, 1,  args.length_tol, args.angle_tol)
# autoindex_test(expt, "n_vertices", 1000, 5000, 500, args.length_tol, args.angle_tol)
autoindex_test(expt, "n_subdiv", 20, 40, 1, int, args.length_tol, args.angle_tol)
autoindex_test(expt, "indexing_tol", 0.05, 0.3, 0.05, float, args.length_tol, args.angle_tol)
