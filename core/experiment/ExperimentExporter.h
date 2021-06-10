//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/ExperimentExporter.h
//! @brief     Defines class ExperimentExporter.
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_EXPERIMENTEXPORTER_H
#define NSX_CORE_EXPERIMENT_EXPERIMENTEXPORTER_H

#include "core/instrument/Diffractometer.h"
#include "core/shape/PeakCollection.h"

namespace nsx {

//! Manages the export of peak information to file.
class ExperimentExporter {
 public:
    //! Create the file
    void createFile(std::string name, std::string diffractometer, std::string path);
    //! Write the data into the current file
    void writeData(const std::map<std::string, DataSet*> data);
    //! Write the peak data into the current file
    void writeInstrument(const Diffractometer* diffractometer);
    //! Write the peak data into the current file
    void writePeaks(const std::map<std::string, PeakCollection*> peakCollections);
    //! write the unit cells into current file
    void writeUnitCells(const std::map<std::string, UnitCell*> unit_cells);
    //! Finish writing the current file
    void finishWrite();

 private:
    //! The current file
    std::string _file_name;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_EXPERIMENTEXPORTER_H
