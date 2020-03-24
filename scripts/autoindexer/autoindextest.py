#!/usr/bin/env python3

import sys
import argparse
from experiment import Parameters, Experiment, pynsxprint
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool autoindexing test script')
parser.add_argument('--name', type=str, dest='name', help='name of system')
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='.tiff raw data files')
parser.add_argument('-n', type=int, dest='nnumors', 
                    help='number of numors to process')
args = parser.parse_args()

filenames = args.files
name = str(args.name)
detector = 'BioDiff5000'
params = Parameters()
nnumors = int(args.nnumors)

nfiles = len(filenames)
frange = len(filenames) - nnumors

for i in range(frange):
    pynsxprint("Trying numors " + str(i) + " to " + str(i+nnumors))
    files = filenames[i:i+nnumors]
    pynsxprint("Files: " + str(files))
    expt = Experiment(name, detector, params)
    pynsxprint("Loading data...")
    expt.load_raw_data(files)
    pynsxprint("...data loaded\n")
    pynsxprint("Finding peaks...")
    expt.find_peaks()
    pynsxprint("...peak finding complete\n")
    pynsxprint("Integrating...")
    expt.integrate_peaks()
    pynsxprint("...integration complete\n")
    pynsxprint("Filtering...")
    expt.filter_peaks()
    pynsxprint("...filtering complete\n")
    pynsxprint("Autoindexing...")
    try:
        expt.autoindex()
    except(RuntimeError):
        pynsxprint("Autoindexing failed, trying next set of numors")
        continue
    pynsxprint("...autoindexing complete\n")
    pynsxprint("Files: " + str(files))
    sys.exit(0)
