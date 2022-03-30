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
struct DataResolution;

//! Manages the export of peak information to file.
class PeakExporter {
 public:
    //! Saves the shell information to file.
    bool saveStatistics(
        std::string filename, const nsx::DataResolution* perShell,
        const nsx::DataResolution* overall);
    //! Saves the peak information to ShelX file format given a peak list.
    bool saveToShelXMerged(const std::string& filename, nsx::MergedData* mergedData);
    //! Saves the peak information to ShelX file format given a merged peak list.
    bool saveToFullProfMerged(const std::string& filename, nsx::MergedData* mergedData);
    //! Saves the peak information to FullProf file format given a peak list.
    bool saveToSCAMerged(
        const std::string& filename, nsx::MergedData* mergedData, double scale = 1.0);
    //! Saves the peak information to FullProf file format given a merged peak list.
    bool saveToShelXUnmerged(const std::string& filename, nsx::MergedData* mergedData);
    //! Saves the peak information to .sca file format given a peak list.
    bool saveToFullProfUnmerged(const std::string& filename, nsx::MergedData* mergedData);
    //! Saves the peak information to .sca file format given a merged peak list.
    bool saveToSCAUnmerged(
        const std::string& filename, nsx::MergedData* mergedData, double scale = 1.0);

 private:
};

} // namespace nsx

#endif // NSX_CORE_STATISTICS_PEAKEXPORTER_H
