//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/output/ExperimentExporter.h
//! @brief     Class to export NSXTool to file
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef CORE_OUTPUT_EXPERIMENTEXPORTER_H
#define CORE_OUTPUT_EXPERIMENTEXPORTER_H

#include "core/experiment/Experiment.h"
#include "core/experiment/DataSet.h"
#include "core/peak/PeakCollection.h"

#include <H5Cpp.h>

namespace nsx {

//! Manages the export of peak information to file.
class ExperimentExporter {

public:

    //! Create the file
    bool createFile(std::string name, std::string diffractometer, std::string path);
    //! Write the data into the current file
    bool writeData(const std::map<std::string, DataSet*> data);
    //! Write the peak data into the current file
    bool writeInstrument(const Diffractometer* diffractometer);
    //! Write the peak data into the current file
    bool writePeaks(const std::map<std::string, PeakCollection*> peakCollections);
    //! write the unit cells into current file
    bool writeUnitCells();
    //! Finish writing the current file
    bool finishWrite();

private:
    //! The current file
    std::string _file_name;
};

} // namespace nsx

#endif //CORE_OUTPUT_EXPERIMENTEXPORTER_H