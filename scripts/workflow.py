#!/usr/bin/env python3

'''
workflow.py

A test script for the entire NSXTool workflow
'''

import argparse
import os.path
from nsx.experiment import Experiment, pynsxprint
from nsx.parameters import Parameters
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool workflow test script')
parser.add_argument('--name', type=str, dest='name', help='name of system')
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='.tiff raw data files')
parser.add_argument('--detector', type=str, dest='detector', default='BioDiff5000',
                    help='Type of detector')
parser.add_argument('--loadnsx', action='store_true', dest='loadnsx', default=False,
                    help='load <name>.nsx')
parser.add_argument('-p', '--parameters', type=str, dest='paramfile',
                    default='parameters', help='File containing experiment paramters')
parser.add_argument('--max_autoindex_frames', type=int, dest='max_autoindex_frames',
                    default=12, help='Maximum number of frames to use for autoindexing')
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
    pynsxprint("No parameters file detected, using defaults")

expt = Experiment(args.name, args.detector, params)
min_autoindex_frames = 7


if not args.loadnsx:
    filenames = args.files
    pynsxprint("Loading data...")
    expt.load_raw_data(filenames[0:min_autoindex_frames])
    pynsxprint("...data loaded\n")
    # Find the unit cell
    pynsxprint("Autoindexing...")
    index = min_autoindex_frames
    while True:
        solution = expt.autoindex(args.length_tol, args.angle_tol)
        if solution:
            break
        else:
            expt.add_raw_data_frames([filenames[index]])
            index += 1

    if solution:
        cell = expt.solution2cell(solution)
        a = cell[1][0]
        b = cell[1][1]
        c = cell[1][2]
        alpha = cell[1][3]
        beta = cell[1][4]
        gamma = cell[1][5]
        cell_str = f'{a:>9.3f}{b:>9.3f}{c:>9.3f}{alpha:>8.3f}{beta:>8.2f}{gamma:>8.2f}'
        pynsxprint("Unit cell:")
        pynsxprint(cell_str)
    else:
        raise RuntimeError("Autoindexing Failed")

    pynsxprint("...autoindexing complete")

    expt.add_raw_data_frames(filenames[index:])
    pynsxprint("Finding peaks...")
    expt.find_peaks([expt.data])
    pynsxprint("...peak finding complete\n")
    pynsxprint("Integrating...")
    npeaks = expt.integrate_peaks()
    pynsxprint("...integration complete\n")

    pynsxprint("Filtering...")
    ncaught = expt.filter_peaks()
    pynsxprint("...filtering complete\n")
    pynsxprint("Filter caught " + str(ncaught) + " of " + str(npeaks) + " peaks")
    expt.accept_solution(expt.filtered_collection, solution)

    pynsxprint(f"Saving experiment to file {expt.nsxfile}")
    expt.save()
else:
    if os.path.isfile(expt.nsxfile):
        pynsxprint(f"{expt.nsxfile} exists, loading experiment")
        expt.load()
    else:
        raise OSError("f{expt.nsxfile} not found")

pynsxprint("Building shape library...")
pynsxprint("...finished building shape library\n")

pynsxprint("Predicting peaks...")
pynsxprint("...finished predicting peaks\n")

pynsxprint("Integrating...")
pynsxprint("...integration complete\n")
