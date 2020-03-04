import glob
import os

import numpy as np
import matplotlib.pyplot as plt

import pynsx as nsx

def setup_experiment(name, instrument):

    experiment = nsx.Experiment(name,instrument)

    return experiment

def read_data(experiment, datasets, **kwargs):

    diff = experiment.getDiffractometer()

    data = []

    min_number_frames = kwargs.get("min_number_frames",1)
    
    files = glob.glob(datasets)

    for f in files:

        reader = nsx.HDF5DataReader(f,diff)

        # Discard the data with a low number of frames (unlikely to be "production" data)
        if (reader.getNFrames() < min_number_frames):
            continue

        # Build the dataset
        dset = nsx.DataSet(reader,diff)

        # Add the data to the experiment
        experiment.addData(dset)

        data.append(dset)

    if not data:
        raise RuntimeError("Empty data list")

    return data

def find_peaks(data, **kwargs):

    kernel_name = kwargs.get("kernel_name","annular")
    kernel_parameters = kwargs.get("kernel_parameters",None)
    blob_min_size = kwargs.get("blob_min_size",30)
    blob_max_size = kwargs.get("blob_max_size",10000)
    integration_confidence = kwargs.get("integration_confidence",0.98)
    search_confidence = kwargs.get("search_confidence",0.67)
    absolute_threshold = kwargs.get("absolute_threshold",True)
    threshold_value = kwargs.get("threshold_value",0.4)

    nrows = data[0].nRows()
    ncols = data[0].nCols()

    kernel = nsx.KernelFactory().create(kernel_name,nrows,ncols)

    kernel_parameters = kernel.parameters()
    if kernel_parameters:
        for p_name, p_value in kernel_parameters.items():
            kernel_parameters[p_name] = p_value
    
    kernel_image = kernel.matrix()
    convolver = nsx.Convolver()
    convolver.setKernel(kernel_image)

    finder = nsx.PeakFinder()
    finder.setHandler(nsx.ProgressHandler())

    finder.setConvolver(convolver)
    finder.setMinComponents(blob_min_size)
    finder.setMaxComponents(blob_max_size)
    finder.setIntegrationConfidence(integration_confidence)
    finder.setSearchConfidence(search_confidence)
    finder.setThresholdType(absolute_threshold)
    finder.setThresholdValue(threshold_value)
    finder.setKernel(kernel)

    peaks = finder.find(data)

    return peaks

def filter_peaks(peaks,**kwargs):

    peak_filter = nsx.PeakFilter()

    filtered_peaks = peaks

    if "selected" in kwargs:
        filtered_peaks = peak_filter.selected(filtered_peaks,kwargs["selected"])

    if "min_sigma" in kwargs:
        filtered_peaks = peak_filter.minSigma(filtered_peaks,kwargs["min_sigma"])

    if "signal_to_noise" in kwargs:
        filtered_peaks = peak_filter.signalToNoise(filtered_peaks,kwargs["signal_to_noise"])

    if "d_min" in kwargs:
        filtered_peaks = peak_filter.dMin(filtered_peaks,kwargs["d_min"])

    if "d_max" in kwargs:
        filtered_peaks = peak_filter.dMax(filtered_peaks,kwargs["d_max"])

    if "significance" in kwargs:
        filtered_peaks = peak_filter.significance(filtered_peaks,kwargs["significance"])

    return filtered_peaks

def find_unit_cells(peaks):

    indexer = nsx.AutoIndexer(nsx.ProgressHandler())

    for peak in peaks:
        indexer.addPeak(peak)

    params = nsx.IndexerParameters()
    indexer.autoIndex(params)

    solutions = indexer.getSolutions()

    cell_parameters = []

    unit_cells = []

    for idx, (uc,score) in enumerate(solutions):

        a     = uc.character().a
        b     = uc.character().b
        c     = uc.character().c
        alpha = np.rad2deg(uc.character().alpha)
        beta  = np.rad2deg(uc.character().beta)
        gamma = np.rad2deg(uc.character().gamma)

        cell_parameters.append((idx,a,b,c,alpha,beta,gamma,score))

        print("idx: {0:3d} --- a = {1:6.2f}  b = {2:6.2f}  c = {3:6.2f}  alpha = {4:6.2f}  beta = {5:6.2f}  gamma = {6:6.2f} --- % indexed = {7:5.2f}".format(*(cell_parameters[-1])))

        unit_cells.append((uc,score))

    return unit_cells

def find_space_group(peaks, unit_cell):

    compatible_space_groups = unit_cell.compatibleSpaceGroups()

    hkls = nsx.MillerIndexList()

    for peak in peaks:
        hkls.push_back(nsx.MillerIndex(peak,unit_cell))

    space_groups = []
    for idx,symbol in enumerate(compatible_space_groups):
        sg = nsx.SpaceGroup(symbol)
        space_groups.append((idx,symbol,100.0*(1.0-sg.fractionExtinct(hkls))))
        print("idx: {0:3d} --- symbol = {1:10s}  --- % non-extincted peaks = {2:6.2f}".format(*(space_groups[-1])))

    return space_groups   

def refine_offsets(data, peaks, unit_cell, **kwargs):

    n_iterations = kwargs.get("n_iterations",200)
    n_batches = kwargs.get("n_batches",20)
    min_peaks_per_dataset = kwargs.get("min_peaks_per_dataset",20)

    refinements = []

    for dataset in data:

        print("Refining parameters for dataset", dataset.filename())

        states = dataset.instrumentStates()
                
        peak_filter = nsx.PeakFilter()
        peaks_per_dataset = peak_filter.dataset(peaks,dataset)
                    
        refiner = nsx.Refiner(unit_cell, peaks_per_dataset, n_batches)
        
        if (len(peaks_per_dataset) < min_peaks_per_dataset):
            print("Too few peaks; skipping")
        
        refiner.refineB()
        
        success = refiner.refine(n_iterations)
        
        refinements.append([dataset, refiner, success])

        print("refinement successful:", success)

    return refinements

def predict_peaks(peaks, dataset, unit_cell, batches):
  
    pred = nsx.PeakPredictor(dataset)

    qshape = pred.averageQShape(peaks)

    wavelength = dataset.diffractometer().source().getSelectedMonochromator().getWavelength()    

    d_values = [1/np.linalg.norm(p.getQ().rowVector()) for p in peaks]

    dmin = min(d_values)
    dmax = max(d_values)

    hkls = unit_cell.generateReflectionsInShell(dmin, dmax, wavelength)       
    
    predicted_peaks = []
    
    for batch in batches:

        bcell = batch.cell()
        preds = pred.predictPeaks(hkls, bcell.reciprocalBasis())
        
        for p in preds:

            if not batch.contains(p.getShape().center()[2,0]):
                continue                

            miller_index = nsx.MillerIndex(p,bcell)
            
            if not miller_index.indexed(bcell.indexingTolerance()):
                continue
                
            q = miller_index.rowVector().dot(bcell.reciprocalBasis())
            shape = nsx.Ellipsoid(q.transpose(), qshape)
            try:
                shape = pred.toDetectorSpace(shape)
            except:
                continue
                
            p.setShape(shape)                
            p.addUnitCell(bcell, True)
            predicted_peaks.append(p)
    
    return predicted_peaks

def integrate_peaks(data, peaks, **kwargs):

    peak_max_radius = kwargs.get("peak_max_radius",5.5)
    background_max_radius = kwargs.get("background_max_radius",10.0)

    for dataset in data:
        dataset.integratePeaks(peaks, peak_max_radius, background_max_radius, nsx.ProgressHandler())

def write_statistics(filename, resolution_shells, space_group_name, **kwargs):

    include_friedel = kwargs.get("include_friedel",True)

    space_group = nsx.SpaceGroup(space_group_name)

    with open(filename,"w") as fout:

        stats = []
    
        fout.write("{:8s} {:8s} {:8s} {:8s} {:8s} {:8s} {:8s} {:8s}\n".format("dmin","dmax","n_peaks","Rmeas","Rmerge","Rpim","CChalf","CCtrue"))
        
        for i in range(resolution_shells.nShells()):
        
            d_shell = resolution_shells.shell(i)
    
            merged = nsx.MergedData(space_group, include_friedel)
    
            for peak in d_shell.peaks:
                merged.addPeak(peak)
                
            r = nsx.RFactor()
            cc = nsx.CC()
        
            r.calculate(merged)
            cc.calculate(merged)
        
            shell_stats = {}
            shell_stats['CChalf'] = cc.CChalf()
            shell_stats['CCtrue'] = cc.CCstar()
            shell_stats['Rmeas'] = r.Rmeas()
            shell_stats['Rmerge'] = r.Rmerge()
            shell_stats['Rpim'] = r.Rpim()
        
            fmt = "{:<8.3f} {:<8.3f} {:<8d} {:<8.3f} {:<8.3f} {:<8.3f} {:<8.3f} {:<8.3f}\n"
            fout.write(fmt.format(d_shell.dmin, d_shell.dmax, len(d_shell.peaks), shell_stats["Rmeas"], shell_stats["Rmerge"], shell_stats["Rpim"], shell_stats["CChalf"], shell_stats["CCtrue"]))

def write_shelx_file(filename, unit_cell, peaks):
    
    peak_filter = nsx.PeakFilter()
    
    filtered_peaks = peaks
    
    filtered_peaks = peak_filter.unitCell(filtered_peaks,unit_cell)
    filtered_peaks = peak_filter.indexed(filtered_peaks,unit_cell,unit_cell.indexingTolerance())
    
    peak_fmt = "{:4d}{:4d}{:4d}{:8.2f}{:8.2f}\n"
    
    with open(filename,"w") as fout:
    
        for peak in filtered_peaks:
                        
            miller_index = nsx.MillerIndex(peak,unit_cell)
            
            hkl = miller_index.rowVector()
            
            intensity = peak.correctedIntensity()
            
            fout.write(peak_fmt.format(hkl[0][0],hkl[0][1],hkl[0][2],intensity.value(),intensity.sigma()))
    
                        
def set_resolution_shells(peaks, **kwargs):

    n_resolution_shells = kwargs.get("n_resolution_shells",10)

    d_values = [1/np.linalg.norm(p.getQ().rowVector()) for p in peaks]

    dmin = min(d_values)
    dmax = max(d_values)

    resolution_shells = nsx.ResolutionShell(dmin, dmax, n_resolution_shells)

    for peak in peaks:
        resolution_shells.addPeak(peak)

    return resolution_shells

def plot_I_vs_q(peak_list):
    qs = []
    Is = []
    
    for peak in peak_list:
        q = np.linalg.norm(peak.getQ().rowVector())
        I = peak.correctedIntensity().value()
        qs.append(q)
        Is.append(I)
    
    plt.figure(figsize=(10,10))
    plt.scatter(qs, Is)
    plt.show()

def plot_Isigma_vs_q(peak_list):
    qs = []
    Is = []
    
    for peak in peak_list:
        q = np.linalg.norm(peak.getQ().rowVector())
        I = peak.correctedIntensity().value()
        sigma = peak.correctedIntensity().sigma()
        qs.append(q)
        Is.append(I/sigma)
    
    plt.figure(figsize=(10,10))
    plt.scatter(qs, Is)
    plt.show()    

def plot_dq_vs_frame(peak_list):
    dqs = []
    frames = []
    
    for i in range(len(peak_list)):
        peak = peak_list[i]
        obs_q = peak.getQ().rowVector()
        uc = peak.activeUnitCell()
        bu = uc.reciprocalBasis()
        hkl = nsx.MillerIndex(peak,uc)
        pred_q = hkl.rowVector().dot(bu)
        dq = np.linalg.norm(pred_q-obs_q)
        
        dqs.append(dq)
        frames.append(peak.getShape().center()[2])
        
    plt.figure(figsize=(10,10))
    plt.scatter(frames, dqs)
    plt.show()

def plot_dx_vs_frame(peak_list, outlier=20):
    dxs = []
    frames = []
    
    for i in range(len(peak_list)):
        peak = peak_list[i]
        obs_x = peak.getShape().center()
        uc = peak.activeUnitCell()
        bu = uc.reciprocalBasis()
        hkl = nsx.MillerIndex(peak,uc)
        pred_q = hkl.rowVector().dot(bu)
        
        predictor = nsx.PeakPredictor(peak.data())
        ellipsoid = nsx.Ellipsoid(pred_q.transpose(), 100.0*np.identity(3)) 
        
        preds = predictor.predictPeaks([nsx.MillerIndex(int(hkl[0,0]), int(hkl[0,1]), int(hkl[0,2]))], uc.reciprocalBasis())
            
        if len(preds) != 1:
            peak.setSelected(False)
            continue
            
        dx = np.linalg.norm(obs_x - preds[0].getShape().center())
        
        if (dx > outlier):
            peak.setSelected(False)
            continue
        
        dxs.append(dx)
        frames.append(peak.getShape().center()[2])

    plt.figure(figsize=(10,10))
    plt.scatter(frames, dxs)
    plt.show()


def find_batch(peak, batches):

    z = peak.getShape().center()[2,0]
    
    for b in batches:
        if b.contains(z):
            return b
        
    return None
    
def reindex(peak_list, batches):          

    new_peaks = []
    
    for peak in peak_list:
        
        batch = find_batch(peak, batches)
        
        if batch is None:
            continue
        
        uc = batch.cell()
        
        miller_index = nsx.MillerIndex(peak,uc)

        if not miller_index.indexed(uc.indexingTolerance()):
            continue

        peak.addUnitCell(uc, True)
        new_peaks.append(peak)
            
    return new_peaks

def filter_peaks1(peaks):
    for peak in peaks:
        if not peak.isSelected():
            continue
            
        if peak.pValue() > 1e-3:
            peak.setSelected(False)
            continue
            
        d = 1.0 / np.linalg.norm(peak.getQ())
        
        if d > 50.0 or d < 2.1:
            peak.setSelected(False)
            continue
            
        profile = nsx.Profile()
        
        if profile.fit(peak.getIntegration().getProjectionPeak()) == False:
            peak.setSelected(False)
            continue

def num_selected_peaks(peaks):
    num_selected = 0
    
    for peak in peaks:
        if peak.isSelected():
            num_selected += 1
            
    return num_selected

def remove_deselected(data):
    peaks = data.getPeaks()
    
    for peak in peaks:
        if not peak.isSelected():
            data.removePeak(peak)



