import sys

import yaml

from nb_helper import *

if __name__ == "__main__":
        
    if len(sys.argv) < 2:
        raise RuntimeError("Invalid number of input arguments")

    parameters_yaml_file = sys.argv[1]

    with open(parameters_yaml_file,"r") as fin:
        
        parameters = yaml.load(fin)

    # Read data
    data = read_data(parameters)

    # Find peaks
    peaks = find_peaks(data,parameters)

    # Filter peaks
    filtered_peaks = filter_peaks(peaks,parameters)

    # Find unit cells
    unit_cells = find_unit_cells(filtered_peaks)        

    # select unit cell
    selected_unit_cell_id = int(input("Please enter selected unit cell id: "))
    unit_cell = unit_cells[selected_unit_cell_id][0]

    for peak in filtered_peaks:
        peak.addUnitCell(unit_cell, True)

    space_groups = find_space_group(filtered_peaks,unit_cell)
    selected_space_group_id = int(input("Please enter selected space group id: "))
    space_group = space_groups[0][1]
    unit_cell.setSpaceGroup(space_group)

    refinements = refine_offsets(data,filtered_peaks,unit_cell,parameters)

#    peak_list = nsx.PeakList()

#    for peak in filtered_peaks:
#        peak_list.push_back(peak)

#    for dataset in data:
#        dataset.integratePeaks(peak_list, 5.0, 8.0, nsx.ProgressHandler())
        
#    compute_statistics(filtered_peaks, nsx.SpaceGroup("P 21"), True)



    def predict_peaks(reference_peaks, data, dmin, dmax, B, batches):    
        pred = nsx.PeakPredictor(data)
        qshape = pred.averageQShape(reference_peaks)
        wavelength = data.diffractometer().getSource().getSelectedMonochromator().getWavelength()    
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
        
        state = p.data().interpolatedState(p.getShape().center()[2,0])
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



