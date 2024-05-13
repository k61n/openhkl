//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/PeakExporter.cpp
//! @brief     Class for peak export to file
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/PeakExporter.h"

#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/experiment/DataQuality.h"
#include "core/experiment/MtzExporter.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"
#include "core/statistics/CC.h"
#include "core/statistics/MergedPeak.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/ResolutionShell.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"

#include <fstream>
#include <iomanip>

// TODO URGENT: lots of code duplication here, needs through cleanup

namespace {

// Comparison operators for sorting peaks by d (descending)
bool peakGreaterThan(ohkl::Peak3D* p1, ohkl::Peak3D* p2)
{
    return p1->d() > p2->d();
}

bool mergedPeakGreaterThan(ohkl::MergedPeak* p1, ohkl::MergedPeak* p2)
{
    return p1->peaks()[0]->d() > p2->peaks()[0]->d();
}

}

namespace ohkl {

PeakExporter::PeakExporter() : _sum_intensities(true)
{
    _export_fmt_strings = {
        {ExportFormat::Mtz, "CCP4 (*.mtz)"},
        {ExportFormat::Phenix, "Phenix (*.sca)"},
        {ExportFormat::ShelX, "ShelX (*.hkl)"},
        {ExportFormat::FullProf, "FullProf (*.hkl)"},
    };
}

bool PeakExporter::saveToShelXUnmerged(
    const std::string& filename, MergedPeakCollection* mergedData)
{
    std::vector<Peak3D*> peak_vector;
    for (const MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (auto* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }
    std::sort(peak_vector.begin(), peak_vector.end(), &peakGreaterThan);

    std::fstream file(filename, std::ios::out);
    if (!file.is_open())
        return false;
    for (int i = 0; i < peak_vector.size(); i++) {
        const Peak3D* peak = peak_vector.at(i);
        MillerIndex miller_index = peak->hkl();
        const Eigen::RowVector3i& hkl = miller_index.rowVector();

        const double intensity = unmergedIntensity(peak).value();
        const double sigma_intensity = unmergedIntensity(peak).sigma();

        file << std::fixed << std::setw(4) << hkl(0) << std::fixed << std::setw(4) << hkl(1)
             << std::fixed << std::setw(4) << hkl(2) << std::fixed << std::setw(14)
             << std::setprecision(4) << intensity << std::fixed << std::setw(14)
             << std::setprecision(4) << sigma_intensity << std::fixed << std::setw(5) << "1"
             << std::endl;
    }
    file.close();
    return true;
}

bool PeakExporter::saveToShelXMerged(const std::string& filename, MergedPeakCollection* mergedData)
{
    std::vector<MergedPeak*> merged_peaks;
    for (auto it = mergedData->mergedPeakSet().begin(); it != mergedData->mergedPeakSet().end();
         ++it)
        merged_peaks.emplace_back(const_cast<MergedPeak*>(&(*it)));
    std::sort(merged_peaks.begin(), merged_peaks.end(), &mergedPeakGreaterThan);

    std::fstream file(filename, std::ios::out);
    for (const MergedPeak* peak : merged_peaks) {
        const auto hkl = peak->index();
        const double intensity = peak->intensity().value();
        const double sigma = peak->intensity().sigma();

        file << std::fixed << std::setw(4) << hkl[0] << std::fixed << std::setw(4) << hkl[1]
             << std::fixed << std::setw(4) << hkl[2] << std::fixed << std::setw(14)
             << std::setprecision(4) << intensity << std::fixed << std::setw(14)
             << std::setprecision(4) << sigma << std::fixed << std::setw(5) << "1" << std::endl;
    }
    file.close();
    return true;
}

bool PeakExporter::saveToFullProfUnmerged(
    const std::string& filename, MergedPeakCollection* mergedData)
{
    std::fstream file(filename, std::ios::out);
    if (!file.is_open())
        return false;

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";

    std::vector<Peak3D*> peak_vector;
    for (const MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (auto* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }
    std::sort(peak_vector.begin(), peak_vector.end(), &peakGreaterThan);

    std::shared_ptr<DataSet> data = peak_vector.at(0)->dataSet();
    double wavelength = data->metadata().key<double>(at_wavelength);
    file << std::fixed << std::setw(8) << std::setprecision(3) << wavelength;
    file << " 0 0" << std::endl;

    for (int i = 0; i < peak_vector.size(); i++) {
        Peak3D* peak = peak_vector.at(i);
        MillerIndex miller_index = peak->hkl();
        const Eigen::RowVector3i& hkl = miller_index.rowVector();
        const double intensity = unmergedIntensity(peak).value();
        const double sigma_intensity = unmergedIntensity(peak).sigma();

        file << std::fixed << std::setw(4) << hkl(0) << std::fixed << std::setw(4) << hkl(1)
             << std::fixed << std::setw(4) << hkl(2) << std::fixed << std::setw(14)
             << std::setprecision(4) << intensity << std::fixed << std::setw(14)
             << std::setprecision(4) << sigma_intensity << std::fixed << std::setw(5) << "1"
             << std::endl;
    }
    file.close();
    return true;
}

bool PeakExporter::saveToFullProfMerged(
    const std::string& filename, MergedPeakCollection* mergedData)
{
    std::fstream file(filename, std::ios::out);
    if (!file.is_open())
        return false;

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";

    std::vector<MergedPeak*> merged_peaks;
    for (auto it = mergedData->mergedPeakSet().begin(); it != mergedData->mergedPeakSet().end();
         ++it)
        merged_peaks.emplace_back(const_cast<MergedPeak*>(&(*it)));
    std::sort(merged_peaks.begin(), merged_peaks.end(), &mergedPeakGreaterThan);

    std::shared_ptr<DataSet> data = merged_peaks[0]->peaks()[0]->dataSet();
    double wavelength = data->metadata().key<double>(at_wavelength);
    file << std::fixed << std::setw(8) << std::setprecision(3) << wavelength;
    file << " 0 0" << std::endl;

    for (MergedPeak* peak : merged_peaks) {
        const auto hkl = peak->index();
        const double intensity = peak->intensity().value();
        const double sigma = peak->intensity().sigma();

        file << std::fixed << std::setw(4) << hkl[0] << std::fixed << std::setw(4) << hkl[1]
             << std::fixed << std::setw(4) << hkl[2] << std::fixed << std::setw(14)
             << std::setprecision(4) << intensity << std::fixed << std::setw(14)
             << std::setprecision(4) << sigma << std::fixed << std::setw(5) << "1"

             << std::endl;
    }
    file.close();
    return true;
}

bool PeakExporter::saveToSCAUnmerged(
    const std::string& filename, MergedPeakCollection* mergedData, sptrUnitCell cell, double scale)
{
    std::fstream file(filename, std::ios::out);
    if (!file.is_open())
        return false;

    const UnitCellCharacter character = cell->character();
    std::string symbol = cell->spaceGroup().symbol();
    std::for_each(symbol.begin(), symbol.end(), [](char& c) { c = ::tolower(c); });
    symbol.erase(std::remove(symbol.begin(), symbol.end(), ' '), symbol.end());

    file << "    1\n\n";
    file << std::fixed << std::setw(10) << std::setprecision(3) << character.a << std::fixed
         << std::setw(10) << std::setprecision(3) << character.b << std::fixed << std::setw(10)
         << std::setprecision(3) << character.c << std::fixed << std::setw(10)
         << std::setprecision(3) << character.alpha / deg << std::fixed << std::setw(10)
         << std::setprecision(3) << character.beta / deg << std::fixed << std::setw(10)
         << std::setprecision(3) << character.gamma / deg << " " << symbol << std::endl;

    for (const MergedPeak& merged_peak : mergedData->mergedPeakSet()) {
        const MillerIndex hkl_rep = merged_peak.index();
        for (Peak3D* unmerged_peak : merged_peak.peaks()) {
            const MillerIndex hkl_orig = unmerged_peak->hkl();
            const double intensity = unmergedIntensity(unmerged_peak).value() * scale;
            const double sigma_intensity = unmergedIntensity(unmerged_peak).sigma() * scale;

            file << std::fixed << std::setw(4) << hkl_orig.h() << std::fixed << std::setw(4)
                 << hkl_orig.k() << std::fixed << std::setw(4) << hkl_orig.l() << std::fixed
                 << std::setw(4) << hkl_rep.h() << std::fixed << std::setw(4) << hkl_rep.k()
                 << std::fixed << std::setw(4) << hkl_rep.l() << " " << std::setprecision(1);

            if (abs(intensity) > 100000 - 1) {
                file << std::fixed << std::setw(7) << std::setprecision(1) << std::scientific
                     << intensity << " ";
            } else {
                file << std::fixed << std::setw(7) << intensity << " ";
            }

            if (abs(sigma_intensity) > 100000 - 1) {
                file << std::fixed << std::setw(7) << std::setprecision(1) << std::scientific
                     << sigma_intensity << std::endl;
            } else {
                file << std::fixed << std::setw(7) << sigma_intensity << std::endl;
            }
        }
    }

    file.close();
    return true;
}

bool PeakExporter::saveToSCAMerged(
    const std::string& filename, MergedPeakCollection* mergedData, sptrUnitCell cell, double scale)
{
    std::fstream file(filename, std::ios::out);
    if (!file.is_open())
        return false;

    std::vector<MergedPeak*> merged_peaks;
    for (auto it = mergedData->mergedPeakSet().begin(); it != mergedData->mergedPeakSet().end();
         ++it)
        merged_peaks.emplace_back(const_cast<MergedPeak*>(&(*it)));
    std::sort(merged_peaks.begin(), merged_peaks.end(), &mergedPeakGreaterThan);

    const UnitCellCharacter character = cell->character();
    std::string symbol = cell->spaceGroup().symbol();
    std::for_each(symbol.begin(), symbol.end(), [](char& c) { c = ::tolower(c); });
    symbol.erase(std::remove(symbol.begin(), symbol.end(), ' '), symbol.end());

    file << "    1\n\n";
    file << std::fixed << std::setw(10) << std::setprecision(3) << character.a << std::fixed
         << std::setw(10) << std::setprecision(3) << character.b << std::fixed << std::setw(10)
         << std::setprecision(3) << character.c << std::fixed << std::setw(10)
         << std::setprecision(3) << character.alpha / deg << std::fixed << std::setw(10)
         << std::setprecision(3) << character.beta / deg << std::fixed << std::setw(10)
         << std::setprecision(3) << character.gamma / deg << " " << symbol << std::endl;

    for (MergedPeak* peak : merged_peaks) {
        const auto hkl = peak->index();
        const double intensity = peak->intensity().value() * scale;
        const double sigma_intensity = peak->intensity().sigma() * scale;

        file << std::fixed << std::setw(4) << hkl(0) << std::fixed << std::setw(4) << hkl(1)
             << std::fixed << std::setw(4) << hkl(2) << " " << std::setprecision(1);

        if (abs(intensity) > 100000 - 1) {
            file << std::fixed << std::setw(7) << std::setprecision(1) << std::scientific
                 << intensity << " ";
        } else {
            file << std::fixed << std::setw(7) << intensity << " ";
        }

        if (abs(sigma_intensity) > 100000 - 1) {
            file << std::fixed << std::setw(7) << std::setprecision(1) << std::scientific
                 << sigma_intensity << std::endl;
        } else {
            file << std::fixed << std::setw(7) << sigma_intensity << std::endl;
        }
    }
    file.close();
    return true;
}

bool PeakExporter::saveToShellX(
    const std::string filename, MergedPeakCollection* merged_data, bool merged)
{
    if (merged)
        return saveToShelXMerged(filename, merged_data);
    else
        return saveToShelXUnmerged(filename, merged_data);
}

bool PeakExporter::saveToFullProf(
    const std::string filename, MergedPeakCollection* merged_data, bool merged)
{
    if (merged)
        return saveToFullProfMerged(filename, merged_data);
    else
        return saveToFullProfUnmerged(filename, merged_data);
}

bool PeakExporter::saveToSCA(
    const std::string filename, MergedPeakCollection* merged_data, sptrUnitCell cell, bool merged,
    double scale)
{
    if (merged)
        return saveToSCAMerged(filename, merged_data, cell, scale);
    else
        return saveToSCAUnmerged(filename, merged_data, cell, scale);
}

bool PeakExporter::exportPeaks(
    ExportFormat fmt, const std::string& filename, MergedPeakCollection* merged_data,
    sptrDataSet data, sptrUnitCell cell, bool merged, bool sum_intensity, double scale,
    std::string comment)
{
    _sum_intensities = sum_intensity;
    switch (fmt) {
        case ExportFormat::Mtz: {
            MtzExporter exporter(merged_data, data, cell, merged, _sum_intensities);
            return exporter.writeToFile(filename);
        }
        case ExportFormat::Phenix: {
            return saveToSCA(filename, merged_data, cell, merged, scale);
        }
        case ExportFormat::ShelX: {
            return saveToShellX(filename, merged_data, merged);
        }
        case ExportFormat::FullProf: {
            return saveToFullProf(filename, merged_data, merged);
        }
        default: return false;
    }
}

Intensity PeakExporter::unmergedIntensity(const Peak3D* peak)
{
    if (_sum_intensities)
        return peak->correctedSumIntensity();
    else
        return peak->correctedProfileIntensity();
}

} // namespace ohkl
