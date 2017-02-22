#!/usr/bin/python
from __future__ import print_function
import sys

if len(sys.argv) != 3:
    print("usage: dos2unix input output")
    exit()

infile = open(sys.argv[1], "r")
outfile = open(sys.argv[2], "w")

for line in infile:
    out = ""
    for c in line:
        if c != '\r': out += c
    outfile.write(out)
