//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/DataQuality.cpp
//! @brief     Handler for computing data quality metrix in Experiment
//!
//! @homepage  ###HOMEPAGE###
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
#include <iostream>

namespace nsx {

void DataQuality::computeQuality(MergedData& merged_peaks)
{
    nobserved = merged_peaks.totalSize();
    nunique = merged_peaks.mergedPeakSet().size();
    redundancy = merged_peaks.redundancy();

    nsx::RFactor rfactor;
    rfactor.calculate(&merged_peaks);
    nsx::CC cc;
    cc.calculate(&merged_peaks);
    Rmerge = rfactor.Rmerge();
    expectedRmerge = rfactor.expectedRmerge();
    Rmeas = rfactor.Rmeas();
    expectedRmeas = rfactor.expectedRmeas();
    Rpim = rfactor.Rpim();
    expectedRpim = rfactor.expectedRpim();
    CChalf = cc.CChalf();
    CCstar = cc.CCstar();
}


void ShellQuality::computeQuality(MergedData& merged_peaks, double d_min, double d_max)
{
    DataQuality::computeQuality(merged_peaks);
    dmin = d_min;
    dmax = d_max;
}

void DataResolution::computeQuality(
    double d_min, double d_max, int n_shells, std::vector<PeakCollection*> collections, 
    SpaceGroup spacegroup, bool friedel)
{
    shells.clear();
    std::vector<ShellQuality> data_resolution;
    ResolutionShell resolution_shell = nsx::ResolutionShell(d_min, d_max, n_shells);
    for (PeakCollection* collection : collections) {
        for (Peak3D* peak : collection->getPeakList())
            resolution_shell.addPeak(peak);
    }

    for (int i = n_shells - 1; i >= 0; --i) {
        double d_lower = resolution_shell.shell(i).dmin;
        double d_upper = resolution_shell.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(spacegroup, friedel);

        for (auto peak : resolution_shell.shell(i).peaks)
            merged_data_per_shell.addPeak(peak);

        ShellQuality quality;
        quality.computeQuality(merged_data_per_shell, d_lower, d_upper);
        quality.setCompleteness(merged_data_per_shell.completeness());
        shells.push_back(quality);
    }
}

void ShellQuality::setCompleteness(const double c)
{
    Completeness = c;
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

void DataQuality::log() const
{
    std::ostringstream oss;
    oss << "Data quality metrics (overall):" << std::endl;
    oss << std::setw(8) << "Rmea" << std::setw(8) << "eRmea" << std::setw(8) << "Rmer"
        << std::setw(8) << "eRmer" << std::setw(8) << "Rpim" << std::setw(8) << "eRpim"
        << std::setw(8) << "CChalf" << std::setw(8) << "Cstar" << std::setw(8) << "Compl.";
    oss << std::endl << toString();
    nsxlog(Level::Info, oss.str());
}

void DataResolution::log() const
{
    std::ostringstream oss;
    oss << "Data quality metrics (per resolution shell):" << std::endl;
    oss << std::setw(8) << "dmin" << std::setw(8) << "dmax" << std::setw(8) << "Rmea"
        << std::setw(8) << "eRmea" << std::setw(8) << "Rmer" << std::setw(8) << "eRmer"
        << std::setw(8) << "Rpim" << std::setw(8) << "eRpim" << std::setw(8) << "CChalf"
        << std::setw(8) << "Cstar" << std::setw(8) << "Compl.";
    for (auto shell : shells) {
        oss << std::endl << shell.toString();
    }
    nsxlog(Level::Info, oss.str());
}

} // namespace nsx
