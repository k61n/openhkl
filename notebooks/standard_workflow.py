import sys

import yaml

import pynsx_workflow

if __name__ == "__main__":
        
    if len(sys.argv) < 2:
        raise RuntimeError("Invalid number of input arguments")


    # Read the workflow parameters
    parameters_yaml_file = sys.argv[1]
    with open(parameters_yaml_file,"r") as fin:
        
        parameters = yaml.load(fin)


    # Setup the experiment
    experiment = pynsx_workflow.setup_experiment(**parameters["experiment"])


    # Read data
    data = pynsx_workflow.read_data(experiment, **parameters["data"])


    # Find peaks
    peaks = pynsx_workflow.find_peaks(data,**parameters["peak_finder"])


    # Filter peaks
    filtered_peaks = pynsx_workflow.filter_peaks(peaks,**parameters["peak_filters"])


    # Find unit cells and set the selected one to the peaks
    unit_cells = pynsx_workflow.find_unit_cells(filtered_peaks)        
    selected_unit_cell_id = int(input("Please enter selected unit cell id: "))
    unit_cell = unit_cells[selected_unit_cell_id][0]
    for peak in filtered_peaks:
        peak.addUnitCell(unit_cell, True)


    # Find space groups and set the selected one to the unit cell
    space_groups = pynsx_workflow.find_space_group(filtered_peaks,unit_cell)
    selected_space_group_id = int(input("Please enter selected space group id: "))
    space_group_name = space_groups[0][1]
    unit_cell.setSpaceGroup(space_group_name)


    # Refine offsets
    refinements = pynsx_workflow.refine_offsets(data,filtered_peaks,unit_cell,**parameters["offset_refiner"])


    # Predict peaks for the whole data
    predicted_peaks = []
    for r in refinements:
        dataset, refiner, success = r
        preds = pynsx_workflow.predict_peaks(filtered_peaks, dataset, unit_cell, refiner.batches())
        predicted_peaks.extend(preds)
    print("Total number of predicted peaks: {:d}\n".format(len(predicted_peaks)))


    # Integrate predicted peaks
    pynsx_workflow.integrate_peaks(data,predicted_peaks,**parameters["peak_integrator"])


    # Filter the predicted peaks
    predicted_peaks = pynsx_workflow.filter_peaks(predicted_peaks,**parameters["peak_filters"])
    print("Total number of filtered peaks: {:d}\n".format(len(predicted_peaks)))


    # Compute global statistics
    resolution_shells = pynsx_workflow.set_resolution_shells(predicted_peaks,1)
    print("Overall statistics:")
    stats = pynsx_workflow.compute_statistics(resolution_shells,space_group_name,**parameters["statistics"])
    print("\n")


    # Compute the statistics per resolution shell
    resolution_shells = pynsx_workflow.set_resolution_shells(predicted_peaks,**parameters["resolution_shells"])
    print("Statistics per resolution shell:")
    stats = pynsx_workflow.compute_statistics(resolution_shells,space_group_name,**parameters["statistics"])

