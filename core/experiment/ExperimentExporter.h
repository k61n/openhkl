//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#include <map>
#include <string>
#include <vector>

namespace nsx {

class DataSet;
class Diffractometer;
class InstrumentStateSet;
class PeakCollection;
class UnitCell;

//! Manages the export of peak information to file.
class ExperimentExporter {
 public:
    //! Create the file
    void createFile(std::string name, std::string diffractometer, std::string path);
    //! Write the data to the current file
    void writeData(const std::map<std::string, DataSet*> data);
    //! Write the peak data to the current file
    void writePeaks(const std::map<std::string, PeakCollection*> peakCollections);
    //! write the unit cells to current file
    void writeUnitCells(const std::vector<UnitCell*> unit_cells);
    //! write the instrument states to the current file
    void writeInstrumentStates(const std::map<DataSet*, InstrumentStateSet*> instrumentStateMap);
    //! Finish writing the current file
    void finishWrite();

 private:
    //! The current file
    std::string _file_name;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_EXPERIMENTEXPORTER_H
