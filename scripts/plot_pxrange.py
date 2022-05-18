#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy as np
import pathlib

files = pathlib.Path('.').glob("pxrange*")
plt.xlabel("Resolution shell")
plt.ylabel("Rmerge")
for file in sorted(files):
    pxrange = int(str(file)[-3:])
    data = []
    with open(file, "r") as infile:
        for line in infile.readlines():
            data.append(float(line.split()[4]))
    plt.plot(data, label=str(pxrange), linewidth=0.5)


plt.yticks(np.arange(0, 1, 0.1))
plt.ylim(0.1, 0.6)
plt.legend()
plt.savefig("trypsin_profile3d_pxrange.pdf")
