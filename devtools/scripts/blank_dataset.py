import h5py
import numpy

scan = 50.0
delta = 0.3
N = int(scan / delta)

with h5py.File("blank.hdf", "w") as f:
    data = f.create_group("Data")
    scan = f.create_group("Data/Scan")
    expr = f.create_group("Experiment")
    info = f.create_group("Info")
    
    data.create_dataset("Counts", (N, 900, 2500,), dtype='i4', compression="gzip", compression_opts=9)
    
    scan.create_dataset("Sample/chi", (N,), dtype="f8")
    omega = scan.create_dataset("Sample/omega", (N,), dtype="f8")
    scan.create_dataset("Sample/phi", (N,), dtype="f8")
    
    scan.create_dataset("Source/chi-mono", (N,), dtype="f8")
    scan.create_dataset("Source/om-mono", (N,), dtype="f8")
    scan.create_dataset("Source/phi-mono", (N,), dtype="f8")
    scan.create_dataset("Source/x-mono", (N,), dtype="f8")
    scan.create_dataset("Source/y-mono", (N,), dtype="f8")
    
    scan.create_dataset("Detector/2theta(gamma)", (N,), dtype="f8")
    
    expr.attrs.create("npdone", N, dtype="i4")
    expr.attrs.create("monitor", 0.0)
    expr.attrs.create("wavelength", 2.67)
    expr.attrs.create("Numor", 0, dtype="i4")
  
    info.attrs.create("Instrument", b"BioDiff")
    
       
    for i in range(N):
        omega[i] = i*delta
