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

#include "core/data/DataTypes.h"

#include <map>
#include <string>

namespace ohkl {

class Intensity;
class Peak3D;
class MergedPeakCollection;

enum class ExportFormat { Mtz = 0, Phenix, ShelX, FullProf, Count };

//! Manages the export of peak information to file.
class PeakExporter {
 public:
    PeakExporter();
    //! Saves the peak information to ShelX file format given a peak list.
    bool saveToShelXMerged(const std::string& filename, MergedPeakCollection* mergedData);
    //! Saves the peak information to ShelX file format given a merged peak list.
    bool saveToFullProfMerged(const std::string& filename, MergedPeakCollection* mergedData);
    //! Saves the peak information to FullProf file format given a peak list.
    bool saveToSCAMerged(
        const std::string& filename, MergedPeakCollection* mergedData, sptrUnitCell cell,
        double scale = 1.0);
    //! Saves the peak information to FullProf file format given a merged peak list.
    bool saveToShelXUnmerged(const std::string& filename, MergedPeakCollection* mergedData);
    //! Saves the peak information to .sca file format given a peak list.
    bool saveToFullProfUnmerged(const std::string& filename, MergedPeakCollection* mergedData);
    //! Saves the peak information to .sca file format given a merged peak list.
    bool saveToSCAUnmerged(
        const std::string& filename, MergedPeakCollection* mergedData, sptrUnitCell cell,
        double scale = 1.0);

    bool saveToShellX(const std::string filename, MergedPeakCollection* merged_data, bool merged);
    bool saveToFullProf(const std::string filename, MergedPeakCollection* merged_data, bool merged);
    bool saveToSCA(
        const std::string filename, MergedPeakCollection* merged_data, sptrUnitCell cell,
        bool merged, double scale = 1.0);

    void setSumIntensities(bool flag) { _sum_intensities = flag; };

    //! Export peaks to the specified format
    bool exportPeaks(
        ExportFormat fmt, const std::string& filename, MergedPeakCollection* merged_data,
        sptrDataSet data, sptrUnitCell cell, bool merged, bool sum_intensity, double scale = 1.0,
        std::string comment = "");

    std::map<ExportFormat, std::string>* exportFormatStrings() { return &_export_fmt_strings; };

 private:
    Intensity unmergedIntensity(const Peak3D* peak);

    std::map<ExportFormat, std::string> _export_fmt_strings;
    bool _sum_intensities; // use pixel sum intensities if true, otherwise profile
};

} // namespace ohkl

#endif // OHKL_CORE_STATISTICS_PEAKEXPORTER_H
