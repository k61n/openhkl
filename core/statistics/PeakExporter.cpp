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
#include "core/data/DataTypes.h"
#include "core/experiment/DataQuality.h"
#include "core/experiment/MtzExporter.h"
#include "core/raw/DataKeys.h"
#include "core/statistics/CC.h"
#include "core/statistics/RFactor.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

#include "core/peak/Peak3D.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/ResolutionShell.h"
#include "tables/crystal/SpaceGroup.h"

#include <fstream>
#include <iomanip>

// TODO URGENT: lots of code duplication here, needs through cleanup

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
    std::fstream file(filename, std::ios::out);
    for (const MergedPeak& peak : mergedData->mergedPeakSet()) {
        const auto hkl = peak.index();
        const double intensity = peak.intensity().value();
        const double sigma = peak.intensity().sigma();

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

    std::vector<Peak3D*> peak_vector;
    for (const MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (auto* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }
    std::shared_ptr<DataSet> data = peak_vector[0]->dataSet();
    double wavelength = data->metadata().key<double>(at_wavelength);
    file << std::fixed << std::setw(8) << std::setprecision(3) << wavelength;
    file << " 0 0" << std::endl;

    for (const MergedPeak& peak : mergedData->mergedPeakSet()) {
        const auto hkl = peak.index();
        const double intensity = peak.intensity().value();
        const double sigma = peak.intensity().sigma();

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

    std::vector<Peak3D*> peak_vector;
    for (const MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (auto* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }
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

    for (int i = 0; i < peak_vector.size(); i++) {
        const Peak3D* peak = peak_vector.at(i);
        const MillerIndex miller_index = peak->hkl();
        const Eigen::RowVector3i& hkl = miller_index.rowVector();
        const double intensity = unmergedIntensity(peak).value() * scale;
        const double sigma_intensity = unmergedIntensity(peak).sigma() * scale;

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

bool PeakExporter::saveToSCAMerged(
    const std::string& filename, MergedPeakCollection* mergedData, sptrUnitCell cell, double scale)
{
    std::fstream file(filename, std::ios::out);
    if (!file.is_open())
        return false;

    std::vector<const Peak3D*> peak_vector;
    for (const MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (const Peak3D* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }
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

    for (const MergedPeak& peak : mergedData->mergedPeakSet()) {
        const auto hkl = peak.index();
        const double intensity = peak.intensity().value() * scale;
        const double sigma_intensity = peak.intensity().sigma() * scale;

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
