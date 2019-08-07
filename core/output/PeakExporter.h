//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/output/PeakExporter.h
//! @brief     Class for peak export to file
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_OUTPUT_PEAKEXPORTER_H
#define CORE_OUTPUT_PEAKEXPORTER_H

#include "core/analyse/MergedData.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/SpaceGroup.h"
#include "core/statistics/ResolutionShell.h"

namespace nsx {

//! Manages the export of peak information to file.
class PeakExporter {

public:

    //! Saves the shell information to file.
    void saveStatistics(
        std::string filename,  
        nsx::ResolutionShell resolutionShell, 
        nsx::SpaceGroup spaceGroup, 
        bool inclFriedel);
    //! Saves the peak information to ShelX file format given a peak list.
    void saveToShelX(
        std::string filename, 
        nsx::PeakList* peakList);
    //! Saves the peak information to ShelX file format given a merged peak list.
    void saveToShelX(
        std::string filename, 
        nsx::MergedData* mergedData);
    //! Saves the peak information to FullProf file format given a peak list.
    void saveToFullProf(
        std::string filename, 
        nsx::PeakList* peakList);
    //! Saves the peak information to FullProf file format given a merged peak list.
    void saveToFullProf(
        std::string filename, 
        nsx::MergedData* mergedData, 
        nsx::PeakList* peakList);
    //! Saves the peak information to .sca file format given a peak list.
    void saveToSCA(
        std::string filename, 
        nsx::PeakList* peakList);
    //! Saves the peak information to .sca file format given a merged peak list.
    void saveToSCA(
        std::string filename, 
        nsx::MergedData* mergedData, 
        nsx::PeakList* peakList);

private:
};

} // namespace nsx

#endif