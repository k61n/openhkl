#!/usr/bin/env python3

'''
workflow.py

A test script for the entire NSXTool workflow
'''

import sys
import argparse
import os.path
from nsx.experiment import Experiment, pynsxprint
from nsx.parameters import Parameters
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool workflow test script')
parser.add_argument('--name', type=str, dest='name', help='name of system', required=True)
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='Data files')
parser.add_argument('--dataformat', type=str, dest='dataformat',
                    help='Format of data files')
parser.add_argument('--detector', type=str, dest='detector',
                    help='Type of detector', required=True)
parser.add_argument('--loadnsx', action='store_true', dest='loadnsx', default=False,
                    help='load <name>.nsx')
parser.add_argument('--predicted', action='store_true', dest='predicted', default=False,
                    help='Saved data in .nsx has completed prediction step')
parser.add_argument('-p', '--parameters', type=str, dest='paramfile',
                    default='parameters', help='File containing experiment paramters')
parser.add_argument('--max_autoindex_frames', type=int, dest='max_autoindex_frames',
                    default=20, help='Maximum number of frames to use for autoindexing')
parser.add_argument('--min_autoindex_frames', type=int, dest='min_autoindex_frames',
                    default=10, help='Minimum number of frames to use for autoindexing')
parser.add_argument('--length_tol', type=float, dest='length_tol',
                    default=1.0, help='length tolerance (a, b, c) for autoindexing')
parser.add_argument('--angle_tol', type=float, dest='angle_tol',
                    default=5.0, help='angle tolerance (alpha, beta, gamma) for autoindexing')
parser.add_argument('--autoindex', action='store_true', dest='autoindex', default=False,
                    help='Autoindex the data')
parser.add_argument('-v', '--verbose', action='store_true', dest='verbose', default=False,
                    help='Print extra output')
parser.add_argument('--batches', action='store', dest='nbatches', type=int, default=20,
                    help='Number of batches for refinement')
args = parser.parse_args()

params = Parameters()
if os.path.isfile(args.paramfile):
    pynsxprint(f"Reading parameters from {args.paramfile}")
    params.load(args.paramfile)
else:
    raise RuntimeError("No parameters file detected")

expt = Experiment(args.name, args.detector, params, verbose=args.verbose)
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
    if args.autoindex:
        # Find/add the unit cell
        pynsxprint("Autoindexing...")
        index = args.min_autoindex_frames
        count = 1
        while index < args.max_autoindex_frames:
            cell_found = False
            try:
                cell_found = expt.autoindex_dataset(data, 0, index, args.length_tol, args.angle_tol)
            except RuntimeError: # Not enough peaks to autoindex?
                index += 1
                count += 1
                continue
            if cell_found:
                break
            else:
                index += 1
                count += 1

        if cell_found:
            pynsxprint("Unit cell:")
            pynsxprint(expt.get_accepted_cell().toString())
        else:
            raise RuntimeError("Autoindexing Failed")
        pynsxprint("...autoindexing complete")
    else:
        a = params.cell['a']
        b = params.cell['b']
        c = params.cell['c']
        alpha = params.cell['alpha']
        beta = params.cell['beta']
        gamma = params.cell['gamma']
        expt.set_unit_cell(a, b, c, alpha, beta, gamma)

    pynsxprint("Finding peaks...")
    expt.find_peaks(data, 0, -1)
    pynsxprint("Integrating...")
    npeaks = expt.integrate_peaks()

    found_peaks = expt.get_found_peaks()
    pynsxprint("Filtering...")
    filtered_collection_name = "filtered"
    params.filter['extinct'] = True
    expt.assign_unit_cell(found_peaks)
    ncaught = expt.filter_peaks(params.filter, found_peaks, filtered_collection_name)
    pynsxprint("Filter caught " + str(ncaught) + " of " + str(npeaks) + " peaks")

    expt.save()
else:
    if not args.predicted:
        expt.load()

if args.predicted:
    expt.load(predicted=args.predicted)
else:
    pynsxprint("Building shape library...")
    all_data = expt.get_data()
    expt.build_shape_library(all_data)
    pynsxprint("Predicting peaks...")
    expt.predict_peaks(all_data, 'None')
    expt.save(predicted=True)

expt.refine(args.nbatches)
expt.check_peak_collections()
pynsxprint("Merging peak collection...")
expt.merge_peaks()
pynsxprint("Computing quality metrics...")
expt.get_statistics()
