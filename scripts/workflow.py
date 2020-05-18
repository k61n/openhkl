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
parser.add_argument('--name', type=str, dest='name', help='name of system', required=True)
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='Data files', required=True)
parser.add_argument('--dataformat', type=str, dest='dataformat',
                    help='Format of data files', required=True)
parser.add_argument('--detector', type=str, dest='detector',
                    help='Type of detector', required=True)
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
    raise RuntimeError("No parameters file detected")

expt = Experiment(args.name, args.detector, params)
all_data = "all"
numors = []
min_autoindex_frames = 7

if not args.loadnsx:
    filenames = args.files
    expt.set_data_format(args.dataformat)
    pynsxprint("Loading data...")
    if args.dataformat == 'raw':
        name = 'all'
        expt.add_data_set(name, filenames)
        numors.append(expt.get_data(name))
    elif args.dataformat == 'nexus':
        for filename in filenames:
            expt.add_data_set(filename, filename)
        numors.append(expt.get_data(filename))
    pynsxprint("...data loaded\n")
    # Find the unit cell
    pynsxprint("Autoindexing...")
    index = min_autoindex_frames
    count = 1
    while index < args.max_autoindex_frames:
        autoindex_files = filenames[0:index]
        set_name = "autoindex" + str(count)
        expt.add_data_set(set_name, autoindex_files)
        data = expt.get_data(set_name)
        cell_found = expt.autoindex(data, args.length_tol, args.angle_tol)
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
    expt.remove_peak_collection(expt.filtered_peaks)

    pynsxprint("Finding peaks...")
    expt.find_peaks(numors)
    pynsxprint("...peak finding complete\n")
    pynsxprint("Integrating...")
    npeaks = expt.integrate_peaks()
    pynsxprint("...integration complete\n")

    pynsxprint("Filtering...")
    ncaught = expt.filter_peaks(params.filter)
    pynsxprint("...filtering complete\n")
    pynsxprint("Filter caught " + str(ncaught) + " of " + str(npeaks) + " peaks")
    # expt.filtered_collection.printUnitCells()

    pynsxprint(f"Saving experiment to file {expt.nsxfile}")
    expt.save()
else:
    if os.path.isfile(expt.nsxfile):
        pynsxprint(f"{expt.nsxfile} exists, loading experiment")
        expt.load()
    else:
        raise OSError("f{expt.nsxfile} not found")

expt.print_unit_cells()
pynsxprint("Building shape library...")
expt.build_shape_library(expt.get_data(all_data))
pynsxprint("...finished building shape library\n")
expt.save()

pynsxprint("Predicting peaks...")
expt.predict_peaks(expt.get_data(all_data), 'None')
pynsxprint("...finished predicting peaks\n")

pynsxprint("Integrating...")
pynsxprint("...integration complete\n")
