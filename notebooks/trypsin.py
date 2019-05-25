#!/usr/bin/env python
# coding: utf-8

# run this script in the directory that contains the raw data file trypsin.hdf

import sys
import pynsx as nsx
import math
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import least_squares

################################################################################
## load configuration and raw data
################################################################################

expt = nsx.Experiment('test', 'BioDiff2500')
biodiff = expt.diffractometer()
reader = nsx.HDF5DataReader("trypsin.hdf", biodiff)
data = nsx.DataSet(reader)
expt.addData(data)
print("data loaded")

## show the data
#plt.figure(figsize=(20,10))
#plt.imshow(np.log(data.frame(0)))
#plt.show()

################################################################################
## find the peaks and their position on the map
################################################################################

print("peakfinder ...")

# get the peakfinder
finder = nsx.PeakFinder()
convoler = nsx.AnnularConvolver()

# set the method as annular (I guess like circular)
finder.setConvolver(convoler)

# set seach parameters
finder.setMinSize(30)
finder.setMaxSize(10000)
finder.setPeakScale(1.0)
finder.setThreshold(80.0)

# find the peaks
peaks = finder.find([data])

# amount of peaks found
print("... peakfinder found", len(peaks), "peaks")
print("Are we good 1:", len(peaks) == 9913)

peak_list = nsx.PeakList()

for peak in peaks:
    peak_list.push_back(peak)

print("integrator ...")
integrator = nsx.PixelSumIntegrator(False, False)
integrator.integrate(peak_list, data, 3.0, 3.0, 6.0)
print("... integrator")

################################################################################
## Set some detector parameters
################################################################################

detector = expt.diffractometer().detector()

# set the parameters
detector.setDistance(0.199)
detector.setWidth(0.199*2*np.pi)
detector.setHeight(0.45)

# autoindex routine
def autoindex(peaks):

    # do the filtering
    indexer = nsx.AutoIndexer(nsx.ProgressHandler())
    num_to_index = 0
    for peak in peaks:
        d = 1.0 / np.linalg.norm(peak.q().rowVector())
        if peak.selected() and d > 1.5 and d < 50.0:
            indexer.addPeak(peak)
            num_to_index += 1

    print("indexing", num_to_index, "peaks")

    print("Are we good 2:", num_to_index == 7798)

    # autoindex
    params = nsx.IndexerParameters()
    handler = nsx.ProgressHandler()
    indexer.autoIndex(params)

    # take the first found unit cell
    soln = indexer.solutions()[0]
    assert(soln[1] > 92.0)
    uc = nsx.UnitCell(soln[0])
    uc.setSpaceGroup("P 21 21 21")
    for peak in peaks:
        peak.setUnitCell(uc)
    ch = uc.character()
    print('a: ', ch.a, 'b: ', ch.b, 'c: ', ch.c)
    print('alpha: ', ch.alpha, 'beta: ', ch.beta, 'gamma: ', ch.gamma)
    print('Spacegroup: ', uc.spaceGroup().symbol())
    return uc

print("autoindex ...")
uc = autoindex(peaks)
print("... autoindex")

def reindex(uc, peaks, tolerance=0.2):
    indexed_peaks = nsx.PeakList()
    for peak in peaks:
        hkl = nsx.MillerIndex(peak.q(), uc)
        if (hkl.indexed(tolerance)):
            indexed_peaks.push_back(peak)
    return indexed_peaks

print("reindex ...")
indexed_peaks = reindex(uc, peaks)
print("... reindex, found", len(indexed_peaks), "indexed peaks")
print("Are we good 3:", len(indexed_peaks) == 7521)


################################################################################
## Refinement functions
################################################################################

def refine(peaks):
    states = data.instrumentStates()
    refiner = nsx.Refiner(states,uc, peaks, int(data.nFrames() / 10))
    refiner.refineUB()
    refiner.refineKi()
    refiner.refineSamplePosition()
    refiner.refineSampleOrientation()
    result = refiner.refine(500)
    print(result)
    return refiner

def fit_residual(params, predicted_qs, npeaks):
    #distance, width, height = params
    #width, height = params
    #height = params[0]
    width = params[0]
    #detector.setDistance(distance)
    detector.setWidth(width)
    #detector.setHeight(height)

    res = []

    for i in npeaks:
        q_pred = predicted_qs[i]
        q_obs = indexed_peaks[i].q().rowVector()
        #print(q_pred, q_obs)
        dq = (q_pred-q_obs)
        res.append(dq[0,0])
        res.append(dq[0,1])
        res.append(dq[0,2])

    return res

def refine_geometry(indexed_peaks):

    npeaks = range(len(indexed_peaks))
    predicted_qs = []

    BU = uc.reciprocalBasis()
    detector = expt.diffractometer().detector()

    for i in npeaks:
        hkl = nsx.MillerIndex(indexed_peaks[i].q(), uc)
        q_pred = hkl.rowVector().dot(BU)
        #print(indexed_peaks[i].q().rowVector(), q_pred)
        predicted_qs.append(q_pred)


    #params0 = detector.distance(), detector.width(), detector.height()
    #params0 = 0.199, 0.199*2*np.pi, 0.45
    detector.setDistance(0.199)
    detector.setWidth(0.199*2*np.pi)
    detector.setHeight(0.45)

    #params0 = detector.width(), detector.height()
    #params0 = detector.height(),
    params0 = detector.width()

    result = least_squares(lambda p: fit_residual(p, predicted_qs, npeaks), params0)

    print(result.success, result.x)

    distance = detector.distance()

    #distance, width, height = result.x
    #width, height = result.x
    #height = result.x[0]
    width = result.x[0]

    detector.setWidth(width)
    #detector.setHeight(height)

refine(indexed_peaks)

################################################################################
## Refine the position and the position of the sample as well as the unit cell parameters
################################################################################

widths = []
heights = []
abc = []

for i in range(10):
    widths.append(detector.width())
    heights.append(detector.height())
    uc = autoindex(peaks)  # UB
    indexed_peaks = reindex(uc, peaks)
    refiner = refine(indexed_peaks)          # sample offset and orientation
    refine_geometry(indexed_peaks) # detector width and height
    for b in refiner.batches():
        ch = b.cell().character()
        abc.append([ch.a, ch.b, ch.c])

plt.plot(range(len(widths)), widths)
plt.show()

plt.plot(range(len(heights)), heights)
plt.show()

plt.plot([a[0] for a in abc])
plt.plot([a[1] for a in abc])
plt.plot([a[2] for a in abc])
plt.show()

################################################################################
## Correct the peak indexing and the reprocess the shape library and actual intensity
################################################################################

fit_peaks = nsx.PeakList()

for peak in peaks:
    if peak.enabled():
        d = 1.0 / np.linalg.norm(peak.q().rowVector())
        inten = peak.correctedIntensity()
        fit_peaks.push_back(peak)

nx = 20
ny = 20
nz = 20

kabsch_coords = True
peak_scale = 3.
sigmaD = 0.4692
sigmaM = 0.3485
sigma = np.array([sigmaD, sigmaD, sigmaM])

aabb = nsx.AABB()
aabb.setLower(-peak_scale * sigma)
aabb.setUpper(peak_scale * sigma)

bkgBegin = 3
bkgEnd = 4.5

shape_library = nsx.ShapeLibrary(kabsch_coords, peak_scale, bkgBegin, bkgEnd)
integrator = nsx.ShapeIntegrator(shape_library, aabb, nx, ny, nz)

integrator.integrate(
    fit_peaks, data, shape_library.peakScale(),
    shape_library.bkgBegin(), shape_library.bkgEnd())

shape_library = integrator.library()

################################################################################
## predict peaks
################################################################################

dmin = 1.5
dmax = 50
radius = 500
n_frames = 5
min_neighbors = 20
peak_interpolation = 1
predicted_peaks =  nsx.predictPeaks(
    shape_library, data, uc, dmin, dmax,
    radius, n_frames, min_neighbors, peak_interpolation)

################################################################################
## Integrate predicted peaks
################################################################################

integrator = nsx.Profile1DIntegrator(shape_library, 500, 5)
dmin = 1.5
dmax = 50
peak_filter = nsx.PeakFilter()
integration_peaks = peak_filter.dRange(predicted_peaks, dmin, dmax)
integrator.integrate(predicted_peaks, data, 3.0, 3.0, 6.0)

################################################################################
## Filter peaks
################################################################################

peak_filter = nsx.PeakFilter()
filtered_peaks = peak_filter.enabled(integration_peaks, True)
filtered_peaks = peak_filter.hasUnitCell(filtered_peaks)
cell = filtered_peaks[0].unitCell()
filtered_peaks = peak_filter.unitCell(filtered_peaks, cell)
filtered_peaks = peak_filter.indexed(filtered_peaks, cell, cell.indexingTolerance())

################################################################################
## Plot the selected peaks in a 3D opengl view
################################################################################

from simpleplot.canvas.multi_canvas import MultiCanvasItem as mc
from PyQt5 import QtWidgets, QtGui, QtCore
from pprint import pprint

# set up the widget
app = QtWidgets.QApplication(sys.argv)
widget          = QtWidgets.QWidget()
multi_canvas    = mc(
    widget = widget,
    grid        = [[True]],
    element_types = [['3D']],
    x_ratios    = [1],
    y_ratios    = [1],
    background  = "w",
    highlightthickness = 0)

base_colors = [[1.,1.,1., 1.],[1.,0.5,0., 1.]]
base_positions = [0.,1]

ax = multi_canvas.getSubplot(0,0)
x_range = 30
y_range = 30
x_scale = 1
y_scale = 1
z_scale = 0.05
offset_scale= 5.

mul = 50

# Selected hkl peaks
points = [2000,2001,2002]
# for i in range(2000,3000):
#     if predicted_peaks[i].selected():
#         points.append(i)
#         if len(points) > 50:
#             break

x_offset = 0
y_offset = 0

# process for selected hkl series
for k, val in enumerate(points):

    #set all hkl parameters
    hkl =  nsx.MillerIndex(predicted_peaks[val].q(), predicted_peaks[val].unitCell())
    h = hkl.h()
    k = hkl.k()
    l = hkl.l()
    I = predicted_peaks[val].correctedIntensity()
    intensity = I.value()
    sigma = I.sigma()
    print('Peak: ', val, h,k,l,intensity, sigma)

    # set all bounding box parameters
    elipsis = predicted_peaks[val].shape()
    center  = elipsis.center()
    region = nsx.IntegrationRegion(
        predicted_peaks[val],
        predicted_peaks[val].peakEnd(),
        predicted_peaks[val].bkgBegin(),
        predicted_peaks[val].bkgEnd() )
    bounding_box = region.peakBB()
    upper  = bounding_box.upper()
    lower  = bounding_box.lower()
    x_bound  = [lower[0][0] - center[0][0], upper[0][0] - center[0][0]]
    y_bound  = [lower[1][0] - center[1][0], upper[1][0] - center[1][0]]
    upper[2][0]  = min([upper[2][0], data.nFrames()-1])
    lower[2][0]  = max([lower[2][0], 0])
    frame_bound  = upper[2][0] - lower[2][0]
    mul = int(frame_bound)
    loop = [int(ll * frame_bound/mul + lower[2][0]) for ll in range(mul+1)]

    x_offset = 0
    surfaces = []
    z_min = []
    z_max = []

    surfaces_base = []
    z_min_base = []
    z_max_base = []

    for l,j in enumerate(loop):
        to_plot = np.array(data.frame(j), dtype='float64')[
            int(lower[1][0]): int(upper[1][0]),
            int(lower[0][0]): int(upper[0][0])]
        base =  np.array(data.frame(j), dtype='float64')[
            int(center[1][0] - x_range): int(center[1][0] + x_range),
            int(center[0][0] - y_range): int(center[0][0] + y_range)]

        if not base.size == 0 and not to_plot.size == 0:
            to_plot -= np.amin(base)
            base -= np.amin(base)
            to_plot *= z_scale
            base *= z_scale
            base[int(lower[1][0]) - int(center[1][0] - x_range): -(int(center[1][0]+x_range)-int(upper[1][0])),
                int(lower[0][0]) - int(center[0][0] - y_range): -(int(center[0][0]+y_range)-int(upper[0][0]))] = np.amin(to_plot)

            z_min.append(np.amin(to_plot))
            z_max.append(np.amax(to_plot))
            z_min_base.append(np.amin(base))
            z_max_base.append(np.amin(to_plot))

            x = (np.array([i + (int(lower[1][0])-int(center[1][0]-x_range)) for i in range(to_plot.shape[0]+1)]))*x_scale + x_offset
            y = (np.array([i + (int(lower[0][0])-int(center[0][0]-x_range)) for i in range(to_plot.shape[1]+1)]))*y_scale + y_offset

            x_base = (np.array([i for i in range(base.shape[0])]))*x_scale + x_offset
            y_base = (np.array([i for i in range(base.shape[1])]))*y_scale + y_offset

            surfaces.append(ax.addPlot(
                'Surface',
                x = x,
                y = y,
                z = to_plot ,
                Name = 'bin'))

            surfaces_base.append(ax.addPlot(
                'Surface',
                x = x_base,
                y = y_base,
                z = base ,
                Positions = base_positions,
                Colors = base_colors,
                Name = 'bin'))

            x_offset += base.shape[0]*x_scale + 2

    # set the color limits for the surfaces
    for surface in surfaces:
        surface.shader_parameters['Bounds z'] = [[False, min(z_min), max(z_max)]]

    # set the y_offset for the following plots
    y_offset += (int(center[0][0] + y_range) - int(center[0][0] - y_range) ) * y_scale +2

# ploting of the data
ax.draw()
widget.show()
sys.exit(app.exec_())

################################################################################
## Print the statistic informations for a given group of peaks
################################################################################

def compute_statistics(peak_list, group, friedel):
    merged = nsx.MergedData(group, friedel)
    for peak in peak_list:
        if peak.selected():
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

print(compute_statistics(peaks, nsx.SpaceGroup("P 21 21 21"), True))
print(compute_statistics(predicted_peaks, nsx.SpaceGroup("P 21 21 21"), True))

################################################################################
## Print the shell informations
################################################################################

nshells = 20
dmin = 1.5
dmax = 50.0

shells = nsx.ResolutionShell(dmin, dmax, nshells)
stats = []

num_good = 0

for peak in predicted_peaks:
    inten = peak.correctedIntensity()

    if (inten.sigma() <= 0):
        continue

    #if (inten.value() / inten.sigma() < 0.1):
    #    continue

    bb = peak.shape().aabb()
    dx = bb.upper()-bb.lower()

    dx = np.linalg.norm(dx)
    if dx > 2000:
        continue

    # detector plates
    c = peak.shape().center()

    if c[0,0] > 1720 and c[0,0] < 1750:
        continue

    shells.addPeak(peak)
    num_good += 1

for i in range(nshells):
    shell = shells.shell(i)
    stats.append(compute_statistics(shell.peaks, nsx.SpaceGroup("P 21 21 21"), True))

def print_stats():
    print(" n      dmin     dmax     Rmeas    Rmerge   Rpim     CChalf   CCtrue")
    for i,s in enumerate(stats):
        shell = shells.shell(i)
        dmin = shell.dmin
        dmax = shell.dmax
        fmt = "{0:4d} {1:8.3f} {2:8.3f} {3:8.3f} {4:8.3f} {5:8.3f} {6:8.3f} {7:8.3f}"
        print(fmt.format(len(shell.peaks), dmin, dmax, s["Rmeas"], s["Rmerge"], s["Rpim"], s["CChalf"], s["CCtrue"]))

print_stats()

################################################################################
## Merge equivalent peaks of the found and predicted peaks.
################################################################################

# parameters
sigma_cut = 1.0
lower_d   = 1.5
upper_d   = 50.

# initialize the peaks
merged_data = nsx.MergedData(nsx.SpaceGroup("P 21 21 21"), True)

# add the strong peaks
for peak in fit_peaks:
    if not peak.selected():
        continue

    inten = peak.correctedIntensity()
    if inten.sigma() < sigma_cut:
        continue

    d = 1.0 / np.linalg.norm(peak.q().rowVector())
    if not d > lower_d or not d < upper_d:
        continue

    merged_data.addPeak(peak)

# add the predicted peaks
for peak in predicted_peaks:
    if not peak.selected():
        continue

    inten = peak.correctedIntensity()
    if inten.sigma() < sigma_cut:
        continue

    d = 1.0 / np.linalg.norm(peak.q().rowVector())
    if not d > lower_d or not d < upper_d:
        continue

    merged_data.addPeak(peak)

################################################################################
## Get information about the unit cell and generate the header for the sca file
################################################################################

# unit cell properties
ch = uc.character()
a = round(ch.a, 3)
b = round(ch.b, 3)
c = round(ch.c, 3)
alpha = round(np.rad2deg(ch.alpha), 4)
beta  = round(np.rad2deg(ch.beta) , 4)
gamma = round(np.rad2deg(ch.gamma), 4)
symbol = uc.spaceGroup().symbol()

# print of unit cell properties
print("a:", a)
print("b:", b)
print("c:", c)
print("alpha:", gamma)
print("beta:" , beta)
print("gamma:", gamma)
print("Bravais:", symbol)

# header
sca_header = '    1\n\n'

temp = ''
for i,element in enumerate([a,b,c,alpha,beta,gamma]):
    str_element = str(element)
    len_element = len(str_element)
    temp += ''.join([' ']*(10 - len_element)) + str_element

sca_header += temp+' '+''.join(symbol.split()).lower()+'\n'


################################################################################
## Generate the content to be put into the sca output file to be read by phenix
################################################################################
sca_data = []

for peak in merged_data.peaks():

    # get the indexes, intensity and sigma
    hkl =  peak.index()
    h = hkl.h()
    k = hkl.k()
    l = hkl.l()
    I = peak.intensity()
    intensity = I.value()
    sigma = I.sigma()

    # generate row
    row = []
    row.append(str(h))
    row.append(str(k))
    row.append(str(l))
    row.append(intensity)
    row.append(sigma)

    # generate the text
    temp = ''
    for i, length in enumerate([4,4,4,8,8]):
        if i < 3:
            temp += ''.join([' ']*(length - len(row[i]))) + row[i]
        else:
            if row[i] > 100000 - 1:
                num = '%.1e'%row[i]
            elif row[i] < -10000 + 1:
                num = '%.0e'%row[i]
            else:
                num = str(round(row[i],1))
            temp += ' ' + ''.join([' ']*(length - 1 - len(num))) + num

    sca_data.append(temp)
sca_data = '\n'.join(sca_data)
sca_data += '\n'

################################################################################
## Write to file
################################################################################

file = open('hkls.sca','w')
file.write(sca_header + sca_data)
file.close()

sca_header
sca_data
