//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/statistics/PeakExporter.h
//! @brief     Class for peak export to file
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_STATISTICS_PEAKEXPORTER_H
#define NSX_CORE_STATISTICS_PEAKEXPORTER_H

#include "core/peak/Peak3D.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/ResolutionShell.h"
#include "tables/crystal/SpaceGroup.h"

namespace nsx {

//! Manages the export of peak information to file.
class PeakExporter {

 public:
    //! Saves the shell information to file.
    void saveStatistics(
        std::string filename, nsx::ResolutionShell resolutionShell, nsx::SpaceGroup spaceGroup,
        bool inclFriedel);
    //! Saves the peak information to ShelX file format given a peak list.
    void saveToShelXMerged(std::string filename, nsx::MergedData* mergedData);
    //! Saves the peak information to ShelX file format given a merged peak list.
    void saveToFullProfMerged(std::string filename, nsx::MergedData* mergedData);
    //! Saves the peak information to FullProf file format given a peak list.
    void saveToSCAMerged(std::string filename, nsx::MergedData* mergedData);
    //! Saves the peak information to FullProf file format given a merged peak list.
    void saveToShelXUnmerged(std::string filename, nsx::MergedData* mergedData);
    //! Saves the peak information to .sca file format given a peak list.
    void saveToFullProfUnmerged(std::string filename, nsx::MergedData* mergedData);
    //! Saves the peak information to .sca file format given a merged peak list.
    void saveToSCAUnmerged(std::string filename, nsx::MergedData* mergedData);

 private:
};

} // namespace nsx

#endif // NSX_CORE_STATISTICS_PEAKEXPORTER_H