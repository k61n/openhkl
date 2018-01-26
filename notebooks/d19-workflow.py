import pynsx as nsx
import math
import numpy as np
from matplotlib import pyplot as plt
from os import path
import glob

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
        assert(pred_q.shape == obs_q.shape)
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

        
    print("npred", len(dxs))
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
        
        hkl = np.array([[0.,0.,0.]])
        uc = batch.cell()
        
        if uc.getMillerIndices(peak.getQ(), hkl):
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

def num_selected_peaks(peaks):
    num_selected = 0
    
    for peak in peaks:
        if peak.isSelected():
            num_selected += 1
            
    return num_selected
    
def filter_peaks(peaks):
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
        
def remove_deselected(data):
    peaks = data.getPeaks()
    
    for peak in peaks:
        if not peak.isSelected():
            data.removePeak(peak)

expt = nsx.Experiment('test', 'D19')
diff = expt.getDiffractometer()

data_dir = path.expanduser("~/data/d19/thom/y-corrected-data/")
data_files = glob.glob(data_dir + "*.h5")

numors = []

for f in data_files[:5]:
    reader = nsx.HDF5DataReader(f,diff)

    # Discard the data with a low number of frames (unlikely to be "production" data)
    if (reader.getNFrames() < 900):
        continue

    # Build the dataset
    data = nsx.DataSet(reader,diff)

    # Add the data to the experiment
    expt.addData(data)

    numors.append(data)

plt.figure(figsize=(20,10))
plt.imshow(np.log(1+numors[0].frame(100)))
plt.show()

nrows = numors[0].nRows()
ncols = numors[0].nCols()

kernel = nsx.AnnularKernel(nrows, ncols)
kernel_image = kernel.getKernel()
convolver = nsx.Convolver()
convolver.setKernel(kernel_image)

finder = nsx.PeakFinder()
finder.setConvolver(convolver)
finder.setMinComponents(30)
finder.setMaxComponents(10000)
finder.setIntegrationConfidence(0.98)
finder.setSearchConfidence(0.67)
finder.setThresholdType(1)
finder.setThresholdValue(0.4)
finder.setKernel(kernel)

num_numors = len(numors)
peaks = finder.find(numors)

for peak in peaks:
       
    inten = peak.correctedIntensity()
    
    if not peak.isSelected():
        continue
    
    if (inten.sigma() < 1.0):
        peak.setSelected(False)

plot_I_vs_q(peaks)

# clean up peaks
good_peaks = []

for peak in peaks:
    I = peak.correctedIntensity().value()
    sigma = peak.correctedIntensity().sigma()
    
    # keep only selected peaks
    if not peak.isSelected():
        continue
    
    # keep only strong peaks for autoindex
    if I < 1.0*sigma:
        continue
        
    d = 1.0 / np.linalg.norm(peak.getQ().rowVector())
    
    # keep peaks only in specified d range
    if d > 10.0 or d < 0.65:
        continue
        
    good_peaks.append(peak)
    
plot_I_vs_q(good_peaks)

####################################
# AUTO-INDEXING
####################################

indexer = nsx.AutoIndexer(nsx.ProgressHandler())

for peak in good_peaks:
    indexer.addPeak(peak)

params = nsx.IndexerParameters()
indexer.autoIndex(params)

solutions = indexer.getSolutions()

cell_parameters = []

for idx, (uc,score) in enumerate(solutions):

    a     = uc.character().a
    b     = uc.character().b
    c     = uc.character().c
    alpha = np.rad2deg(uc.character().alpha)
    beta  = np.rad2deg(uc.character().beta)
    gamma = np.rad2deg(uc.character().gamma)

    cell_parameters.append((idx,a,b,c,alpha,beta,gamma,score))

    print("idx: {0:3d} --- a = {1:6.2f}  b = {2:6.2f}  c = {3:6.2f}  alpha = {4:6.2f}  beta = {5:6.2f}  gamma = {6:6.2f} --- % indexed = {7:5.2f}".format(*(cell_parameters[-1])))

# take best unit cell
uc = solutions[0][0]

for peak in good_peaks:
    peak.addUnitCell(uc, True)

####################################
# SPACE-GROUP
####################################

compatible_space_groups = uc.compatibleSpaceGroups()

hkls = nsx.MillerIndexList()

for peak in good_peaks:
    hkls.push_back(nsx.MillerIndex(peak,uc))

space_groups = []
for idx,symbol in enumerate(compatible_space_groups):
    sg = nsx.SpaceGroup(symbol)
    space_groups.append((idx,symbol,100.0*(1.0-sg.fractionExtinct(hkls))))
    print("idx: {0:3d} --- symbol = {1:10s}  --- % non-extincted peaks = {2:6.2f}".format(*(space_groups[-1])))

peak_set = nsx.PeakSet()

for peak in good_peaks:
    peak_set.insert(peak)

for data in numors:
    data.integratePeaks(peak_set, 5.0, 8.0, nsx.ProgressHandler())
    
compute_statistics(good_peaks, nsx.SpaceGroup("P 21"), True)

#################
# Refinement
#################
refinements = []

for data in numors:
    print("Refining parameters for dataset", data.getFilename())
    nbatches = 2
    states = data.getInstrumentStates()
    
    data_peaks = []
    
    for peak in good_peaks:
        if peak.data().getFilename() == data.getFilename():
            data_peaks.append(peak)
                
    refiner = nsx.Refiner(uc, data_peaks, nbatches)
    
    if (len(data_peaks) < 20):
        print("Too few peaks; skipping")
    
    #refiner.refineSamplePosition(states)
    #refiner.refineKi(states)
    #refiner.refineSampleOrientation(states)
    
    #refiner.refineU()
    refiner.refineB()
    
    success = refiner.refine(200)
    
    refinements.append([data, refiner, success])
    print("refinement successful:", success)

def predict_peaks(reference_peaks, data, dmin, dmax, B, batches):    
    pred = nsx.PeakPredictor(data)
    qshape = pred.averageQShape(reference_peaks)
    wavelength = data.getDiffractometer().getSource().getSelectedMonochromator().getWavelength()    
    hkls = uc.generateReflectionsInShell(dmin, dmax, wavelength)       
    prediction = pred.predictPeaks(hkls, B)
    
    good_pred = []
    
    for batch in batches:
        bcell = batch.cell()
        preds = pred.predictPeaks(hkls, bcell.reciprocalBasis())
        
        for p in preds:
            if not batch.contains(p.getShape().center()[2,0]):
                continue
                
            hkl = np.array([[0.,0.,0.]])
            
            if not bcell.getMillerIndices(p.getQ(), hkl):
                continue
                
            q = hkl.dot(bcell.reciprocalBasis())
            shape = nsx.Ellipsoid(q.transpose(), qshape)
            try:
                shape = pred.toDetectorSpace(shape)
            except:
                continue
                
            p.setShape(shape)                
            p.addUnitCell(bcell, True)
            good_pred.append(p)
    
    return good_pred

predicted = []

for r in refinements:
    data, refiner, success = r
    preds = predict_peaks(good_peaks, data, 0.65, 10.0, uc.reciprocalBasis(), refiner.batches())
    predicted.extend(preds)
    
print("Total predicted peaks:", len(predicted))

peak_set = nsx.PeakSet()

for peak in predicted:
    peak.setSelected(True)
    peak_set.insert(peak)

for data in numors:
    data.integratePeaks(peak_set, 5.5, 10.0, nsx.ProgressHandler())
    
compute_statistics(predicted, nsx.SpaceGroup("P 21"), True)

nshells = 10
dmin = 0.65
dmax = 8.1

shells = nsx.ResolutionShell(dmin, dmax, nshells)
stats = []

for peak in predicted:
    shells.addPeak(peak)
    
for i in range(nshells):
    shell = shells.shell(i)
    stats.append(compute_statistics(shell, nsx.SpaceGroup("P 21"), True))
   
def print_stats():    
    print("   dmin     dmax     Rmeas    Rmerge   Rpim     CChalf   CCtrue")
    d = shells.getD()

    for i,s in enumerate(stats):
        
        fmt = "{0:8.3f} {1:8.3f} {2:8.3f} {3:8.3f} {4:8.3f} {5:8.3f} {6:8.3f}"
        print(fmt.format(d[i], d[i+1], s["Rmeas"], s["Rmerge"], s["Rpim"], s["CChalf"], s["CCtrue"]))
    
print_stats()

good_predicted = []
lors = []
qs = []

for p in predicted:
    inten = p.correctedIntensity()
    
    state = p.data().getInterpolatedState(p.getShape().center()[2,0])
    lor = state.lorentzFactor(nsx.DirectVector(p.getShape().center()))
    lors.append(lors)
    qs.append(np.linalg.norm(p.getQ().rowVector()))
    
    # integration failure
    if inten.sigma() < 1.0:        
        continue
        
    good_predicted.append(p)

plot_I_vs_q(good_predicted)
plot_Isigma_vs_q(good_predicted)

nshells = 10
dmin = 0.65
dmax = 8.1

shells = nsx.ResolutionShell(dmin, dmax, nshells)
stats = []

for peak in good_predicted:
    shells.addPeak(peak)
    
for i in range(nshells):
    shell = shells.shell(i)
    stats.append(compute_statistics(shell, nsx.SpaceGroup("P 21"), True))

print_stats()



