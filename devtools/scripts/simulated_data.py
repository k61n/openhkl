###############################################################################
# 
# author: Jonathan Fisher
#         j.fisher@fz-juelich.de
#         jonathan.m.fisher@gmail.com
#
# description:
#     generates (obviously fake) simulated biodiff data,
#  with and without slowly varying background. This is used
#  in functional tests of core.
#
###############################################################################

import numpy as np
import math
import pynsx as nsx
import h5py

pi2 = np.pi / 2.0
infile = "/home/jonathan/git/nsxtool/tests/data/simulated.hdf"
outfile = "/home/jonathan/git/nsxtool/tests/data/generated.hdf"

from matplotlib import pyplot as plt

def correct_image(image):
    baseline = 227.0
    gain = 8.0
    return gain*(image+baselin)

def poisson_noise(image):
    return np.sqrt(image)*np.random.normal(0, 1, image.shape)

exp = nsx.Experiment("Simulated", "BioDiff")
diff = exp.getDiffractometer()
reader = nsx.HDF5DataReader(infile, diff)
data = nsx.DataSet(reader, diff)

uc = nsx.UnitCell(45.0, 55.0, 65.0, pi2, pi2, pi2)
uc.setSpacegroup("P 21 21 21")

det_shape = nsx.Ellipsoid([800.0, 450.0, 10.0], 5.0)
peak = nsx.Peak3D
