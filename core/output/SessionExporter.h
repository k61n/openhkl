//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/output/SessionExporter.h
//! @brief     Class to export NSXTool to file
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#ifndef CORE_OUTPUT_SESSIONEXPORTER_H
#define CORE_OUTPUT_SESSIONEXPORTER_H

#include "core/experiment/Experiment.h"
#include "core/analyse/MergedData.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

//! Manages the export of peak information to file.
class SessionExporter {

public:

    // //! Create the file
    // void createFile(std::string path);
    // //! Grab all the session items and list them for save
    // void createSessionList()


private:

    

};

} // namespace nsx

#endif