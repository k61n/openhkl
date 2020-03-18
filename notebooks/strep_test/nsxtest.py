#!/usr/bin/env python3

import argparse
from experiment import Parameters, Experiment

parser = argparse.ArgumentParser(description='NSX Tool testing script')
parser.add_argument('--name', '-n', type=str, nargs=1, dest='name',
                    help='name of system')
parser.add_argument('--file', '-f', type=str, nargs='+', dest='files',
                    help='.tiff raw data files')
args = parser.parse_args()

filenames = args.files
name = str(args.name)
detector = 'BioDiff5000'

params = Parameters()
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
