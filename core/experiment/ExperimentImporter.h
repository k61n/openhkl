//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/ExperimentImporter.h
//! @brief     Imports the Experiment saved by ExperimentExporter
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_EXPERIMENTIMPORTER_H
#define NSX_CORE_EXPERIMENT_EXPERIMENTIMPORTER_H

#include <string>

namespace nsx {

class Experiment;

//! Manages the import of peak information to file.
class ExperimentImporter {
 public:
    //! Create the file
    void setFilePath(std::string path, Experiment* experiment);
    //! read data from the current file
    void loadData(Experiment* experiment);
    //! read peaks from the current file
    void loadPeaks(Experiment* experiment);
    //! read unit cells from the current file
    void loadUnitCells(Experiment* experiment);
    //! read instrument states from the current file
    void loadInstrumentStates(Experiment* experiment);
    //! Finish writing the current file
    void finishLoad();

 private:
    //! The current file
    std::string _file_name;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_EXPERIMENTIMPORTER_H
