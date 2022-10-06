//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/PeakExporter.h
//! @brief     Class for peak export to file
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_STATISTICS_PEAKEXPORTER_H
#define OHKL_CORE_STATISTICS_PEAKEXPORTER_H

#include "tables/crystal/UnitCell.h"
#include <string>

namespace ohkl {

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
        std::string filename, const ohkl::DataResolution* perShell,
        const ohkl::DataResolution* overall);
    //! Saves the peak information to ShelX file format given a peak list.
    bool saveToShelXMerged(const std::string& filename, ohkl::MergedData* mergedData);
    //! Saves the peak information to ShelX file format given a merged peak list.
    bool saveToFullProfMerged(const std::string& filename, ohkl::MergedData* mergedData);
    //! Saves the peak information to FullProf file format given a peak list.
    bool saveToSCAMerged(
        const std::string& filename, ohkl::MergedData* mergedData, sptrUnitCell cell,
        double scale = 1.0);
    //! Saves the peak information to FullProf file format given a merged peak list.
    bool saveToShelXUnmerged(const std::string& filename, ohkl::MergedData* mergedData);
    //! Saves the peak information to .sca file format given a peak list.
    bool saveToFullProfUnmerged(const std::string& filename, ohkl::MergedData* mergedData);
    //! Saves the peak information to .sca file format given a merged peak list.
    bool saveToSCAUnmerged(
        const std::string& filename, ohkl::MergedData* mergedData, sptrUnitCell cell,
        double scale = 1.0);

 private:
};

} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_PEAKEXPORTER_H
