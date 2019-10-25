//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/ExperimentImporter.h
//! @brief     Imports the Experiment saved by ExperimentExporter
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOADER_EXPERIMENTIMPORTER_H
#define CORE_LOADER_EXPERIMENTIMPORTER_H

#include "core/experiment/Experiment.h"
#include "core/experiment/DataSet.h"
#include "core/peak/PeakCollection.h"

#include <H5Cpp.h>

namespace nsx {

//! Manages the import of peak information to file.
class ExperimentImporter {

public:

    //! Create the file
    bool setFilePath(std::string path, Experiment* experiment);
    //! Write the data into the current file
    bool loadData(Experiment* experiment);
    //! Write the peak data into the current file
    bool loadPeaks(Experiment* experiment);
    //! write the unit cells into current file
    bool loadUnitCells(Experiment* experiment);
    //! Finish writing the current file
    bool finishLoad();

private:
    //! The current file
    std::string _file_name;
};

} // namespace nsx

#endif //CORE_OUTPUT_EXPERIMENTEXPORTER_H