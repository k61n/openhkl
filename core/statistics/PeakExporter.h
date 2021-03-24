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

#include <string>

namespace nsx {

class Peak3D;
class MergedData;
class ResolutionShell;
class SpaceGroup;

//! Manages the export of peak information to file.
class PeakExporter {
 public:
    //! Saves the shell information to file.
    void saveStatistics(
        std::string filename, nsx::ResolutionShell resolutionShell, nsx::SpaceGroup spaceGroup,
        bool inclFriedel);
    //! Saves the peak information to ShelX file format given a peak list.
    void saveToShelXMerged(const std::string& filename, nsx::MergedData* mergedData);
    //! Saves the peak information to ShelX file format given a merged peak list.
    void saveToFullProfMerged(const std::string& filename, nsx::MergedData* mergedData);
    //! Saves the peak information to FullProf file format given a peak list.
    void saveToSCAMerged(const std::string& filename, nsx::MergedData* mergedData);
    //! Saves the peak information to FullProf file format given a merged peak list.
    void saveToShelXUnmerged(const std::string& filename, nsx::MergedData* mergedData);
    //! Saves the peak information to .sca file format given a peak list.
    void saveToFullProfUnmerged(const std::string& filename, nsx::MergedData* mergedData);
    //! Saves the peak information to .sca file format given a merged peak list.
    void saveToSCAUnmerged(const std::string& filename, nsx::MergedData* mergedData);

 private:
};

} // namespace nsx

#endif // NSX_CORE_STATISTICS_PEAKEXPORTER_H
