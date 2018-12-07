#!/usr/bin/env python3

import sys

import yaml

import pynsx_utils

if __name__ == "__main__":
        
    if len(sys.argv) < 2:
        raise RuntimeError("Invalid number of input arguments")


    # Read the workflow parameters
    parameters_yaml_file = sys.argv[1]
    with open(parameters_yaml_file,"r") as fin:
        
        parameters = yaml.load(fin)

    # Setup the experiment
    experiment = pynsx_utils.setup_experiment(**parameters["experiment"])


    # Read data
    data = pynsx_utils.read_data(experiment, **parameters["data"])


    # Find peaks
    peaks = pynsx_utils.find_peaks(data,**parameters["peak_finder"])


    # Filter peaks
    filtered_peaks = pynsx_utils.filter_peaks(peaks,**parameters["peak_filters"])


    # Find unit cells and set the selected one to the peaks
    unit_cells = pynsx_utils.find_unit_cells(filtered_peaks)
    if parameters["auto_indexing"]["select_best_unit_cell"]:        
        selected_unit_cell_id = 0
    else:
        selected_unit_cell_id = int(input("Please enter selected unit cell id: "))
    print("Selected unit cell: {:d}".format(selected_unit_cell_id))
    unit_cell = unit_cells[selected_unit_cell_id][0]
    for peak in filtered_peaks:
        peak.addUnitCell(unit_cell, True)

    # Find space groups and set the selected one to the unit cell
    space_groups = pynsx_utils.find_space_group(filtered_peaks,unit_cell)
    if "space_group" in parameters["space_group_definition"]:
        selected_space_group_id = None
        for idx,(_,sg,_) in enumerate(space_groups):
             if sg==parameters["space_group_definition"]["space_group"]:
                 selected_space_group_id = idx
                 break
        if selected_space_group_id is None:
            print("Invalid space group")
        
    else:                
        if parameters["space_group_definition"].get("select_best_space_group",False):
            selected_space_group_id = 0
        else:
            selected_space_group_id = int(input("Please enter selected space group id: "))


    print("Selected space group id: {:d}".format(selected_space_group_id))
    space_group_name = space_groups[selected_space_group_id][1]
    unit_cell.setSpaceGroup(space_group_name)

    # Refine offsets
    refinements = pynsx_utils.refine_offsets(data,filtered_peaks,unit_cell,**parameters["offset_refiner"])

    # Predict peaks for the whole data
    predicted_peaks = []
    for r in refinements:
        dataset, refiner, success = r
        preds = pynsx_utils.predict_peaks(filtered_peaks, dataset, unit_cell, refiner.batches())
        predicted_peaks.extend(preds)
    print("Total number of predicted peaks: {:d}\n".format(len(predicted_peaks)))


    # Integrate predicted peaks
    pynsx_utils.integrate_peaks(data,predicted_peaks,**parameters["peak_integrator"])


    # Filter the predicted peaks
    predicted_peaks = pynsx_utils.filter_peaks(predicted_peaks,**parameters["peak_filters"])
    print("Total number of filtered peaks: {:d}\n".format(len(predicted_peaks)))

    # Compute global statistics
    resolution_shells = pynsx_utils.set_resolution_shells(predicted_peaks, n_resolution_shells=1)
    pynsx_utils.write_statistics(experiment.getName()+"_overall_stat.txt",resolution_shells,space_group_name,**parameters["statistics"])

    # Compute the statistics per resolution shell
    resolution_shells = pynsx_utils.set_resolution_shells(predicted_peaks,**parameters["statistics"])
    pynsx_utils.write_statistics(experiment.getName()+"_stat_per_shell.txt",resolution_shells,space_group_name,**parameters["statistics"])

    pynsx_utils.write_shelx_file(experiment.getName()+"_peak_list.hkl", unit_cell, filtered_peaks)
