//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/DataQuality.cpp
//! @brief     Handler for computing data quality metrix in Experiment
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/DataQuality.h"

#include "base/utils/Logger.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/CC.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/ResolutionShell.h"
#include "tables/crystal/SpaceGroup.h"

#include <iomanip>

namespace ohkl {

void DataQuality::computeQuality(MergedData& merged_peaks, bool sum_intensities)
{
    nobserved = merged_peaks.totalSize();
    int max_peaks = merged_peaks.maxPeaks();
    nunique = merged_peaks.nUnique();
    redundancy = merged_peaks.redundancy();
    Completeness = static_cast<double>(nunique) / static_cast<double>(max_peaks);

    ohkl::RFactor rfactor(sum_intensities);
    rfactor.calculate(&merged_peaks);
    ohkl::CC cc;
    cc.calculate(&merged_peaks);
    Rmerge = rfactor.Rmerge();
    expectedRmerge = rfactor.expectedRmerge();
    Rmeas = rfactor.Rmeas();
    expectedRmeas = rfactor.expectedRmeas();
    Rpim = rfactor.Rpim();
    expectedRpim = rfactor.expectedRpim();
    CChalf = cc.CChalf();
    CCstar = cc.CCstar();

    sum_statistics = sum_intensities;
}


void ShellQuality::computeQuality(
    MergedData& merged_peaks, double d_min, double d_max, bool sum_intensities)
{
    DataQuality::computeQuality(merged_peaks, sum_intensities);
    dmin = d_min;
    dmax = d_max;
}

void DataResolution::addShell(const ShellQuality& shell)
{
    shells.push_back(shell);
}

void DataResolution::clear()
{
    shells.clear();
}

std::string DataQuality::toString() const
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4) << std::setw(8) << Rmeas << std::setw(8)
        << expectedRmeas << std::setw(8) << Rmerge << std::setw(8) << expectedRmerge << std::setw(8)
        << Rpim << std::setw(8) << expectedRpim << std::setw(8) << CChalf << std::setw(8) << CCstar
        << std::setw(8) << Completeness;
    return oss.str();
}

std::string ShellQuality::toString() const
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(4) << std::setw(8) << dmin << std::setw(8) << dmax
        << DataQuality::toString();
    return oss.str();
}

std::string DataResolution::summary() const
{
    std::ostringstream oss;
    oss << "Data quality metrics (per resolution shell):" << std::endl;
    oss << std::setw(8) << "dmin" << std::setw(8) << "dmax" << std::setw(8) << "Rmea"
        << std::setw(8) << "eRmea" << std::setw(8) << "Rmer" << std::setw(8) << "eRmer"
        << std::setw(8) << "Rpim" << std::setw(8) << "eRpim" << std::setw(8) << "CChalf"
        << std::setw(8) << "CCstar" << std::setw(8) << "Compl.";
    for (auto shell : shells) {
        oss << std::endl << shell.toString();
    }
    return oss.str();
}

void DataQuality::log() const
{
    std::ostringstream oss;
    if (sum_statistics)
        oss << "Sum integrated";
    else
        oss << "Profile integrated";
    oss << " metrics (overall):" << std::endl;
    oss << std::setw(8) << "Rmea" << std::setw(8) << "eRmea" << std::setw(8) << "Rmer"
        << std::setw(8) << "eRmer" << std::setw(8) << "Rpim" << std::setw(8) << "eRpim"
        << std::setw(8) << "CChalf" << std::setw(8) << "CCstar" << std::setw(8) << "Compl.";
    oss << std::endl << toString();
    ohklLog(Level::Info, oss.str());
}

void DataResolution::log() const
{
    std::ostringstream oss;
    if (shells[0].sum_statistics)
        oss << "Sum integrated";
    else
        oss << "Profile integrated";
    oss << "metrics (per resolution shell):" << std::endl;
    oss << std::setw(8) << "dmin" << std::setw(8) << "dmax" << std::setw(8) << "Rmea"
        << std::setw(8) << "eRmea" << std::setw(8) << "Rmer" << std::setw(8) << "eRmer"
        << std::setw(8) << "Rpim" << std::setw(8) << "eRpim" << std::setw(8) << "CChalf"
        << std::setw(8) << "CCstar" << std::setw(8) << "Compl.";
    for (auto shell : shells) {
        oss << std::endl << shell.toString();
    }
    ohklLog(Level::Info, oss.str());
}

} // namespace ohkl
