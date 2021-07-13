//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/statistics/PeakExporter.cpp
//! @brief     Class for peak export to file
//!
//! @homepage  ###HOMEPAGE###
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
#include "core/raw/DataKeys.h"
#include "core/statistics/CC.h"
#include "core/statistics/RFactor.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

#include "core/peak/Peak3D.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/ResolutionShell.h"
#include "tables/crystal/SpaceGroup.h"

#include <fstream>
#include <iomanip>

// TODO URGENT: lots of code duplication here, needs through cleanup

namespace nsx {

void PeakExporter::saveStatistics(
    std::string filename, const nsx::DataResolution* perShell, const nsx::DataResolution* overall)
{
    std::fstream file(filename, std::ios::out);

    file << std::fixed << std::setw(10) << "dmax" << std::fixed << std::setw(10) << "dmin"
         << std::fixed << std::setw(10) << "nobs" << std::fixed << std::setw(10) << "nmerge"
         << std::fixed << std::setw(11) << "Redundancy" << std::fixed << std::setw(10) << "R meas."
         << std::fixed << std::setw(12) << "R exp. meas." << std::fixed << std::setw(11)
         << "R merge" << std::fixed << std::setw(13) << "R exp. merge" << std::fixed
         << std::setw(10) << "Rpim" << std::fixed << std::setw(11) << "Rpim exp." << std::fixed
         << std::setw(10) << "CC half" << std::fixed << std::setw(10) << "CC star" << std::endl;

    for (const auto& shell : perShell->shells) {
        file << std::fixed << std::setw(10) << std::setprecision(2) << shell.dmin << std::fixed
             << std::setw(10) << std::setprecision(2) << shell.dmax << std::fixed << std::setw(10)
             << shell.nobserved << std::fixed << std::setw(10) << shell.nunique << std::fixed
             << std::setw(11) << std::setprecision(3) << shell.redundancy << std::fixed
             << std::setw(10) << std::setprecision(3) << shell.Rmeas << std::fixed << std::setw(12)
             << std::setprecision(3) << shell.expectedRmeas << std::fixed << std::setw(11)
             << std::setprecision(3) << shell.Rmerge << std::fixed << std::setw(13)
             << std::setprecision(3) << shell.expectedRmerge << std::fixed << std::setw(10)
             << std::setprecision(3) << shell.Rpim << std::fixed << std::setw(11)
             << std::setprecision(3) << shell.expectedRpim << std::fixed << std::setw(10)
             << std::setprecision(3) << shell.CChalf << std::fixed << std::setw(10)
             << std::setprecision(3) << shell.CCstar << std::endl;
    }

    for (const auto& shell : overall->shells) {
        file << std::fixed << std::setw(10) << std::setprecision(2) << shell.dmin << std::fixed
             << std::setw(10) << std::setprecision(2) << shell.dmax << std::fixed << std::setw(10)
             << shell.nobserved << std::fixed << std::setw(10) << shell.nunique << std::fixed
             << std::setw(11) << std::setprecision(3) << shell.redundancy << std::fixed
             << std::setw(10) << std::setprecision(3) << shell.Rmeas << std::fixed << std::setw(12)
             << std::setprecision(3) << shell.expectedRmeas << std::fixed << std::setw(11)
             << std::setprecision(3) << shell.Rmerge << std::fixed << std::setw(13)
             << std::setprecision(3) << shell.expectedRmerge << std::fixed << std::setw(10)
             << std::setprecision(3) << shell.Rpim << std::fixed << std::setw(11)
             << std::setprecision(3) << shell.expectedRpim << std::fixed << std::setw(10)
             << std::setprecision(3) << shell.CChalf << std::fixed << std::setw(10)
             << std::setprecision(3) << shell.CCstar << std::endl;
    }

    file.close();
}

void PeakExporter::saveToShelXUnmerged(const std::string& filename, nsx::MergedData* mergedData)
{
    std::vector<Peak3D*> peak_vector;
    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (auto* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }

    std::fstream file(filename, std::ios::out);
    for (int i = 0; i < peak_vector.size(); i++) {
        const nsx::Peak3D* peak = peak_vector.at(i);
        if (peak->enabled()) {
            const nsx::UnitCell* cell = peak->unitCell();
            if (cell) {
                nsx::MillerIndex miller_index(peak->q(), *cell);
                if (miller_index.indexed(cell->indexingTolerance())) {
                    const Eigen::RowVector3i& hkl = miller_index.rowVector();
                    const double intensity = peak->correctedIntensity().value();
                    const double sigma_intensity = peak->correctedIntensity().sigma();

                    file << std::fixed << std::setw(4) << hkl(0) << std::fixed << std::setw(4)
                         << hkl(1) << std::fixed << std::setw(4) << hkl(2) << std::fixed
                         << std::setw(14) << std::setprecision(4) << intensity << std::fixed
                         << std::setw(14) << std::setprecision(4) << sigma_intensity << std::fixed
                         << std::setw(5) << "1" << std::endl;
                }
            }
        }
    }
    file.close();
}

void PeakExporter::saveToShelXMerged(const std::string& filename, nsx::MergedData* mergedData)
{
    std::fstream file(filename, std::ios::out);
    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()) {
        const auto hkl = peak.index();
        nsx::Intensity I = peak.intensity();
        const double intensity = I.value();
        const double sigma = I.sigma();

        file << std::fixed << std::setw(4) << hkl[0] << std::fixed << std::setw(4) << hkl[1]
             << std::fixed << std::setw(4) << hkl[2] << std::fixed << std::setw(14)
             << std::setprecision(4) << intensity << std::fixed << std::setw(14)
             << std::setprecision(4) << sigma << std::fixed << std::setw(5) << "1" << std::endl;
    }
    file.close();
}

void PeakExporter::saveToFullProfUnmerged(const std::string& filename, nsx::MergedData* mergedData)
{
    std::fstream file(filename, std::ios::out);

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";

    std::vector<Peak3D*> peak_vector;
    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (auto* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }

    std::shared_ptr<nsx::DataSet> data = peak_vector.at(0)->dataSet();
    double wavelength = data->reader()->metadata().key<double>(nsx::at_wavelength);
    file << std::fixed << std::setw(8) << std::setprecision(3) << wavelength;
    file << " 0 0" << std::endl;

    for (int i = 0; i < peak_vector.size(); i++) {
        nsx::Peak3D* peak = peak_vector.at(i);
        if (peak->enabled()) {
            const nsx::UnitCell* cell = peak->unitCell();
            if (cell) {
                nsx::MillerIndex miller_index(peak->q(), *cell);
                if (miller_index.indexed(cell->indexingTolerance())) {
                    const Eigen::RowVector3i& hkl = miller_index.rowVector();
                    const double intensity = peak->correctedIntensity().value();
                    const double sigma_intensity = peak->correctedIntensity().sigma();

                    file << std::fixed << std::setw(4) << hkl(0) << std::fixed << std::setw(4)
                         << hkl(1) << std::fixed << std::setw(4) << hkl(2) << std::fixed
                         << std::setw(14) << std::setprecision(4) << intensity << std::fixed
                         << std::setw(14) << std::setprecision(4) << sigma_intensity << std::fixed
                         << std::setw(5) << "1" << std::endl;
                }
            }
        }
    }
    file.close();
}

void PeakExporter::saveToFullProfMerged(const std::string& filename, nsx::MergedData* mergedData)
{
    std::fstream file(filename, std::ios::out);

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";

    std::vector<Peak3D*> peak_vector;
    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (auto* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }
    std::shared_ptr<nsx::DataSet> data = peak_vector[0]->dataSet();
    double wavelength = data->reader()->metadata().key<double>(nsx::at_wavelength);
    file << std::fixed << std::setw(8) << std::setprecision(3) << wavelength;
    file << " 0 0" << std::endl;

    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()) {
        const auto hkl = peak.index();
        nsx::Intensity I = peak.intensity();
        const double intensity = I.value();
        const double sigma = I.sigma();

        file << std::fixed << std::setw(4) << hkl[0] << std::fixed << std::setw(4) << hkl[1]
             << std::fixed << std::setw(4) << hkl[2] << std::fixed << std::setw(14)
             << std::setprecision(4) << intensity << std::fixed << std::setw(14)
             << std::setprecision(4) << sigma << std::fixed << std::setw(5) << "1"

             << std::endl;
    }
    file.close();
}

void PeakExporter::saveToSCAUnmerged(const std::string& filename, nsx::MergedData* mergedData)
{
    std::fstream file(filename, std::ios::out);

    std::vector<Peak3D*> peak_vector;
    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (auto* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }
    const UnitCell* unitCell = peak_vector[0]->unitCell();
    const UnitCellCharacter character = unitCell->character();
    std::string symbol = unitCell->spaceGroup().symbol();
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
        const nsx::Peak3D* peak = peak_vector.at(i);
        if (peak->enabled()) {
            const nsx::UnitCell* cell = peak->unitCell();
            if (cell) {
                const nsx::MillerIndex miller_index(peak->q(), *cell);
                if (miller_index.indexed(cell->indexingTolerance())) {
                    const Eigen::RowVector3i& hkl = miller_index.rowVector();
                    const double intensity = peak->correctedIntensity().value();
                    const double sigma_intensity = peak->correctedIntensity().sigma();

                    file << std::fixed << std::setw(4) << hkl(0) << std::fixed << std::setw(4)
                         << hkl(1) << std::fixed << std::setw(4) << hkl(2) << " "
                         << std::setprecision(1);

                    if (abs(intensity) > 100000 - 1) {
                        file << std::fixed << std::setw(7) << std::setprecision(1)
                             << std::scientific << intensity << " ";
                    } else {
                        file << std::fixed << std::setw(7) << intensity << " ";
                    }

                    if (abs(sigma_intensity) > 100000 - 1) {
                        file << std::fixed << std::setw(7) << std::setprecision(1)
                             << std::scientific << sigma_intensity << std::endl;
                    } else {
                        file << std::fixed << std::setw(7) << sigma_intensity << std::endl;
                    }
                }
            }
        }
    }
    file.close();
}

void PeakExporter::saveToSCAMerged(const std::string& filename, nsx::MergedData* mergedData)
{
    std::fstream file(filename, std::ios::out);

    std::vector<const Peak3D*> peak_vector;
    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()) {
        for (const Peak3D* unmerged_peak : peak.peaks())
            peak_vector.push_back(unmerged_peak);
    }
    const UnitCell* unitCell = peak_vector.at(0)->unitCell();
    const UnitCellCharacter character = unitCell->character();
    std::string symbol = unitCell->spaceGroup().symbol();
    std::for_each(symbol.begin(), symbol.end(), [](char& c) { c = ::tolower(c); });
    symbol.erase(std::remove(symbol.begin(), symbol.end(), ' '), symbol.end());

    file << "    1\n\n";
    file << std::fixed << std::setw(10) << std::setprecision(3) << character.a << std::fixed
         << std::setw(10) << std::setprecision(3) << character.b << std::fixed << std::setw(10)
         << std::setprecision(3) << character.c << std::fixed << std::setw(10)
         << std::setprecision(3) << character.alpha / deg << std::fixed << std::setw(10)
         << std::setprecision(3) << character.beta / deg << std::fixed << std::setw(10)
         << std::setprecision(3) << character.gamma * deg << " " << symbol << std::endl;

    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()) {
        const auto hkl = peak.index();
        nsx::Intensity I = peak.intensity();
        const double intensity = I.value();
        const double sigma_intensity = I.sigma();

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
}

} // namespace nsx
