#!/usr/bin/env python3

import sys
import argparse
from experiment import Parameters, Experiment, pynsxprint
from autoindextest import AutoIndexTest
from pdb import set_trace

parser = argparse.ArgumentParser(description='NSXTool autoindexing test script')
parser.add_argument('--name', type=str, dest='name', help='name of system')
parser.add_argument('--files', type=str, nargs='+', dest='files',
                    help='.tiff raw data files')
parser.add_argument('-n', '--nnumors', type=int, dest='nnumors', 
                    help='number of numors to process')
parser.add_argument('--length_tol', type=float, dest='length_tol', default=1.0,
                    help='Tolerance for cell lengths (a, b, c)')
parser.add_argument('--angle_tol', type=float, dest='angle_tol', default=0.1,
                    help='Tolerance for cell angles (alpha, beta, gamma)')
parser.add_argument('--detector', type=str, dest='detector', default='BioDiff5000',
                    help='Name of detector')
args = parser.parse_args()

filenames = args.files
params = Parameters()

nfiles = len(filenames)
frange = len(filenames) - args.nnumors
ref_cell = (46.426, 94.062, 104.008, 90.0, 90.0, 90.0)

cells = []
for i in range(frange):
    pynsxprint("Trying numors " + str(i) + " to " + str(i+args.nnumors))
    numors = range(i,i+args.nnumors)
    files = filenames[i:i+args.nnumors]
    pynsxprint("Files: " + str(files))

    indexer = AutoIndexTest(str(i), args.detector, params)
    indexer.set_ref_cell(*ref_cell)
    indexer.set_tolerance(args.length_tol, args.angle_tol)
    try:
        indexer.autoindex(files, numors)
    except RuntimeError:
        print("Autoindexing failed, continuing...")
        continue
    good_cells = indexer.check_unit_cells()
    cells.extend(good_cells)

for cell in cells:
    print(cell)
