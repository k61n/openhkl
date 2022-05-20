#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy as np
import pathlib

files = pathlib.Path('.').glob("frange_*")
plt.xlabel("Resolution shell")
plt.ylabel("Rmerge")
for file in sorted(files):
    frange = int(str(file).split("_")[1])
    data = []
    with open(file, "r") as infile:
        for line in infile.readlines():
            data.append(float(line.split()[4]))
    plt.plot(data, label=str(frange), linewidth=0.5)


plt.yticks(np.arange(0, 1, 0.1))
plt.ylim(0.1, 0.6)
plt.legend()
plt.savefig("trypsin_profile3d_frange.pdf")
