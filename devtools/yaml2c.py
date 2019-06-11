#!/usr/bin/env python3
# -*- mode: python -*-

# Enclose YAML source lines as "...\n" so that they can be included in C/C++ code.

import re, sys

def edit( fn ):
    # read in
    fd = open( fn, 'r' )
    ti = fd.read()
    fd.close
    t = ti

    # transform
    t = re.sub( r'^', '"', t, flags=re.M )
    t = re.sub( r'$', r'\\n"', t, flags=re.M )

    # save result
    if t==ti:
        return 0
    print( "Changed " + fn )
    fd = open( fn, 'w' )
    fd.write( t )
    fd.close
    return 1

## Main

flist = sys.argv[1:]
ntot = len(flist)
nmod = 0
for fn in flist:
    nmod += edit( fn )

print("Changed %i/%i files" % (nmod, ntot))
