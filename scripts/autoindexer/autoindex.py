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
from experiment import Parameters, Experiment, pynsxprint
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool autoindexing test script')
parser.add_argument('--name', type=str, dest='name', help='name of system')
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='.tiff raw data files')
args = parser.parse_args()

filenames = args.files
name = args.name
detector = 'BioDiff5000'
params = Parameters()

nfiles = len(filenames)

expt = Experiment(name, detector, params)
if os.path.isfile(expt.nsxfile):
    pynsxprint(f"{expt.nsxfile} exists, loading experiment")
    expt.load()
else:
    pynsxprint("Loading data...")
    expt.load_raw_data(filenames)
    pynsxprint("...data loaded\n")
    pynsxprint("Finding peaks...")
    expt.find_peaks()
    pynsxprint("...peak finding complete\n")
    pynsxprint("Integrating...")
    expt.integrate_peaks()
    pynsxprint("...integration complete\n")
    pynsxprint(f"Saving experiment to file {expt.nsxfile}")
    expt.save()

pynsxprint("Filtering...")
expt.filter_peaks()
pynsxprint("...filtering complete\n")
pynsxprint("Autoindexing...")
expt.autoindex()
pynsxprint("...autoindexing complete\n")
