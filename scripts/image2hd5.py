# Author: Jonathan Fisher  j.fisher@fz-juelich.de


from __future__ import print_function
import numpy as np
from matplotlib import pyplot as plt
import glob
import sys
import os
import h5py

def find_range(pattern, first, last):
    potential_matches = sorted(glob.glob(pattern))
    matches = []
    found_first = False
    found_last = False

    for name in potential_matches:
        if name == first:
            found_first = True

        if found_first:
            matches.append(name)

        if name == last:
            break

    if (matches[-1] != last):
        print("warning: did not find last file!")
        
    return matches

        
def get_pattern(first, last):
    dirname = os.path.dirname(first)

    if ( dirname != os.path.dirname(last) ):
        print("Error: first and last files should reside in same directory")
        return None

    extension = os.path.splitext(first)[1]

    if ( extension != os.path.splitext(last)[1] ):
        print("Error: first and last files must have the same extension")
        return None

    if ( extension != ".tiff" and extension != ".tif"):
        print("Warning: image extension unrecognized, may give incorrect behaviour")

    pattern = dirname + "/*" + extension
    return pattern

def check_consistency(output, matches):

    print("Testing consistency of output...")
    
    idx = 0
    f = h5py.File(output, "r")

    for m in matches:
        print("Checking frame %i" % idx)
        
        ref_data = plt.imread(m)
        data = f["/Data/Counts"][idx]

        if ( len(ref_data) != len(data) ):
            print("ERROR: frame %i has %i rows but expected %i" % (idx, len(ref_data), len(data)) )
            exit(1)

        if ( len(ref_data[0]) != len(data[0]) ):
            print("ERROR: frame %i has %i cols but expected %i" % (idx, len(ref_data[0]), len(data[0])) )
            exit(1)

        diff = ref_data - data
        norm2 = diff.dot(diff.transpose()).trace()

        if ( norm2 > 1e-5 ):
            print("ERROR: the HDF5 data does not agree with the reference image")
            exit(1)
        
        idx = idx+1
    
    f.close()

    print("Everything is consistent!")
    

def run_main(argc, argv):

    argument_names = [
        "first",
        "last",
        "instrument",
        "wavelength",
        "delta-chi",
        "delta-omega",
        "delta-phi",
        "output"
        ]
    
    if ( argc != (1+len(argument_names)) ):
        usage_str = "Usage: image2hd5.py"

        for arg in argument_names:
            usage_str = usage_str + " " + arg

        print(usage_str)
        return

    arguments = {}

    for i in range(len(argument_names)):
        name = argument_names[i]
        value = argv[i+1]
        arguments[name] = value
    

    first = arguments["first"]
    last = arguments["last"]
    instrument = arguments["instrument"]
    wavelength = float(arguments["wavelength"])
    delta_chi = float(arguments["delta-chi"])
    delta_omega = float(arguments["delta-omega"])
    delta_phi = float(arguments["delta-phi"])
    output = arguments["output"]

    pattern = get_pattern(first, last)

    if ( pattern == None ):
        print("Could not define the pattern to match against")
        return

    matches = find_range(pattern, first, last)

    f = h5py.File(output, "w", compression="lzf")
    idx = 0

    image = plt.imread(matches[0])
    nframes = len(matches)
    nrows = len(image)
    ncols = len(image[0])

    f.create_dataset("/Data/Counts", shape=(nframes, nrows, ncols), dtype=image.dtype)
    f.create_dataset("/Data/Scan/Detector/2theta(gamma)", shape=(nframes,))
    f.create_dataset("/Data/Scan/Sample/chi", shape=(nframes,))
    f.create_dataset("/Data/Scan/Sample/omega", shape=(nframes,))
    f.create_dataset("/Data/Scan/Sample/phi", shape=(nframes,))
    f.create_dataset("/Data/Scan/Source/chi-mono", shape=(nframes,))
    f.create_dataset("/Data/Scan/Source/om-mono", shape=(nframes,))
    f.create_dataset("/Data/Scan/Source/phi-mono", shape=(nframes,))
    f.create_dataset("/Data/Scan/Source/x-mono", shape=(nframes,))
    f.create_dataset("/Data/Scan/Source/y-mono", shape=(nframes,))

    f.create_group("/Experiment")
    f.create_group("/Info")

    f["/Experiment"].attrs["wavelength"] = np.float64(wavelength)
    f["/Experiment"].attrs["npdone"] = np.int32(nframes)
    f["/Experiment"].attrs["monitor"] = np.float64(0.0)
    f["/Experiment"].attrs["Numor"] = np.int32(0)

    f["/Info"].attrs["Instrument"] = instrument


    print("Writing %i frames of (nrows, ncols) = (%i, %i)" % (nframes, nrows, ncols) )

    for m in matches:
        sys.stdout.write("Reading %s..." % m)
        image = plt.imread(m)
        sys.stdout.write(" Writing to HDF5....\n")
        f["/Data/Counts"][idx] = image

        f["/Data/Scan/Sample/chi"][idx] = idx*delta_chi
        f["/Data/Scan/Sample/omega"][idx] = idx*delta_omega
        f["/Data/Scan/Sample/phi"][idx] = idx*delta_phi

        idx = idx+1

    sys.stdout.write("Flushing to disk...")

    f.flush()
    f.close()

    sys.stdout.write("Done!\n")

    #check_consistency(output, matches)



if __name__ == '__main__':
    run_main(len(sys.argv), sys.argv)
        
