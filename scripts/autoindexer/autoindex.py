#!/usr/bin/env python3

import sys
import argparse
from experiment import Parameters, Experiment
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool autoindexing test script')
parser.add_argument('--name', type=str, nargs=1, dest='name',
                    help='name of system')
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='.tiff raw data files')
args = parser.parse_args()

filenames = args.files
name = str(args.name)
detector = 'BioDiff5000'
params = Parameters()

nfiles = len(filenames)

expt = Experiment(name, detector, params)
print("Loading data...")
expt.load_raw_data(filenames)
print("...data loaded\n")
print("Finding peaks...")
expt.find_peaks()
print("...peak finding complete\n")
print("Integrating...")
expt.integrate_peaks()
print("...integration complete\n")
print("Filtering...")
expt.filter_peaks()
print("...filtering complete\n")
print("Autoindexing...")
expt.autoindex()
print("...autoindexing complete\n")
