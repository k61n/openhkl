import glob
import os

import numpy as np
import matplotlib.pyplot as plt

import pynsx as nsx

def read_data(parameters):

    expt_node = parameters["experiment"]

    data_node = parameters["data"]

    expt = nsx.Experiment(expt_node["name"], expt_node["instrument"])

    diff = expt.getDiffractometer()

    datasets = glob.glob(data_node["datasets"])

    data = []

    for f in datasets[:5]:

        reader = nsx.HDF5DataReader(f,diff)

        # Discard the data with a low number of frames (unlikely to be "production" data)
        if (reader.getNFrames() < data_node["min_number_frames"]):
            continue

        # Build the dataset
        dset = nsx.DataSet(reader,diff)

        # Add the data to the experiment
        expt.addData(dset)

        data.append(dset)

    if not data:
        raise RuntimeError("Empty data list")

    return data

def find_peaks(data,parameters):

    peak_find_node = parameters["peak_finder"]

    nrows = data[0].nRows()
    ncols = data[0].nCols()

    kernel_node = peak_find_node["kernel"]

    kernel = nsx.KernelFactory().create(kernel_node["name"],nrows,ncols)

    kernel_parameters = kernel.parameters()
    for p_name, p_value in kernel_node["parameters"].items():
        kernel_parameters[p_name] = p_value
    

    kernel_image = kernel.matrix()
    convolver = nsx.Convolver()
    convolver.setKernel(kernel_image)

    finder = nsx.PeakFinder()
    finder.setHandler(nsx.ProgressHandler())

    finder.setConvolver(convolver)
    finder.setMinComponents(peak_find_node["blob_min_size"])
    finder.setMaxComponents(peak_find_node["blob_max_size"])
    finder.setIntegrationConfidence(peak_find_node["integration_confidence"])
    finder.setSearchConfidence(peak_find_node["search_confidence"])
    finder.setThresholdType(peak_find_node["absolute_threshold"])
    finder.setThresholdValue(peak_find_node["threshold_value"])
    finder.setKernel(kernel)

    peaks = finder.find(data)

    return peaks

def filter_peaks(peaks,parameters):

    peak_filters_node = parameters["peak_filters"]

    peak_filter = nsx.PeakFilter()

    filtered_peaks = peaks

    filtered_peaks = peak_filter.selected(filtered_peaks)

    filtered_peaks = peak_filter.minSigma(filtered_peaks,peak_filters_node["min_sigma"])

    filtered_peaks = peak_filter.highSignalToNoise(filtered_peaks,peak_filters_node["i_over_sigma"])

    filtered_peaks = peak_filter.dRange(filtered_peaks,peak_filters_node["d_min"],peak_filters_node["d_max"])

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

def refine_offsets(data, peaks, unit_cell, parameters):

    refiner_node = parameters["offset_refiner"]

    refinements = []

    for dataset in data:

        print("Refining parameters for dataset", dataset.filename())

        states = dataset.instrumentStates()
                
        peak_filter = nsx.PeakFilter()
        peaks_per_dataset = peak_filter.dataset(peaks,dataset)
                    
        refiner = nsx.Refiner(unit_cell, peaks_per_dataset, refiner_node["n_batches"])
        
        if (len(peaks_per_dataset) < refined_node["min_peaks_per_dataset"]):
            print("Too few peaks; skipping")
        
        refiner.refineB()
        
        success = refiner.refine(refiner_node["n_iterations"])
        
        refinements.append([dataset, refiner, success])

        print("refinement successful:", success)

    return refinements

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

def compute_statistics(peak_list, group, friedel):

    merged = nsx.MergedData(group, friedel)

    for peak in peak_list:
        if peak.isSelected():
            merged.addPeak(peak)
            
    r = nsx.RFactor()
    cc = nsx.CC()
    
    r.calculate(merged)
    cc.calculate(merged)
    
    stats = {}
    stats['CChalf'] = cc.CChalf()
    stats['CCtrue'] = cc.CCstar()
    stats['Rmeas'] = r.Rmeas()
    stats['Rmerge'] = r.Rmerge()
    stats['Rpim'] = r.Rpim()
    
    return stats

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



