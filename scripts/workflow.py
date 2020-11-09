#!/usr/bin/env python3

'''
workflow.py

A test script for the entire NSXTool workflow
'''

import sys
import argparse
from pathlib import Path
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
parser.add_argument('--max_autoindex_frames', type=int, dest='frames_max',
                    default=20, help='Maximum number of frames to use for autoindexing')
parser.add_argument('--min_autoindex_frames', type=int, dest='frames_min',
                    default=10, help='Minimum number of frames to use for autoindexing')
parser.add_argument('--length_tol', type=float, dest='length_tol',
                    default=0.5, help='length tolerance (a, b, c) for autoindexing')
parser.add_argument('--angle_tol', type=float, dest='angle_tol',
                    default=0.5, help='angle tolerance (alpha, beta, gamma) for autoindexing')
parser.add_argument('-v', '--verbose', action='store_true', dest='verbose', default=False,
                    help='Print extra output')
parser.add_argument('--batches', action='store', dest='nbatches', type=int, default=20,
                    help='Number of batches for refinement')
parser.add_argument('--integrator', action='store', dest='integrator', type=str,
                    default="Profile3D", help="Integrator to use")
args = parser.parse_args()

integrators = {"Profile1D": "1d profile integrator",
               "Profile3D": "3d profile integrator",
               "PixelSum": "Pixel sum integrator",
               "Gaussian": "Gaussian integrator",
               "ISigma": "I/Sigma integrator"}

params = Parameters()
if Path(args.paramfile).is_file():
    pynsxprint(str(f"Reading parameters from {args.paramfile}"))
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
    filenames = [Path(file) for file in args.files]
    pynsxprint("Loading data...")
    expt.add_data_set(filenames, args.dataformat)
    data = expt.get_data()

    pynsxprint("Finding peaks...")
    expt.find_peaks(data, 0, -1)
    pynsxprint("Integrating...")
    npeaks = expt.integrate_peaks(integrators["PixelSum"])

    expt.save()
else:
    if not args.predicted:
        pynsxprint("Loading data...")
        expt.load()

if args.predicted:
    pynsxprint("Loading data...")
    expt.load(predicted=args.predicted)
else:
    # Autoindex
    pynsxprint("Autoindexing...")
    peaks = expt.get_found_peaks()
    success = expt.run_auto_indexer(peaks, args.length_tol, args.angle_tol,
                                    args.frames_min, args.frames_max)
    pynsxprint("Autoindexing successful")
    if not success:
        raise RuntimeError("AutoIndexer failed")
    pynsxprint("Filtering...")
    filtered_collection_name = "filtered"
    peaks = expt.get_found_peaks()
    expt.assign_unit_cell(peaks)
    params.filter['extinct'] = True
    npeaks = peaks.numberOfPeaks()
    ncaught = expt.filter_peaks(params.filter, peaks, filtered_collection_name)
    pynsxprint("Building shape collection...")
    all_data = expt.get_data()
    expt.build_shape_collection(all_data)
    pynsxprint("Predicting peaks...")
    if not args.integrator in integrators:
        raise RuntimeError(f'{args.integrator} is not a valid integrator')
    pynsxprint("Integrating predicted peaks...")
    expt.predict_peaks(all_data, integrators[args.integrator], 'None')
    expt.save(predicted=True)

pynsxprint("Refining cell and instrument states...")
expt.refine(args.nbatches, integrators[args.integrator])
expt.check_peak_collections()
pynsxprint("Merging peak collection...")
expt.merge_peaks()
pynsxprint("Computing quality metrics...")
expt.get_statistics()
