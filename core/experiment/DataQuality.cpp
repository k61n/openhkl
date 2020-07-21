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

#include "DataQuality.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/CC.h"
#include "core/statistics/MergedData.h"
#include "core/statistics/ResolutionShell.h"
#include "tables/crystal/SpaceGroup.h"

namespace nsx {

void DataQuality::computeQuality(MergedData& merged_peaks)
{
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
    dmin = d_max;
}

void DataResolution::computeQuality(double d_min, double d_max, int n_shells,
                                    PeakCollection* predicted, PeakCollection* found,
                                    SpaceGroup spacegroup, bool friedel)
{
    std::vector<ShellQuality> data_resolution;
    ResolutionShell resolution_shell = nsx::ResolutionShell(d_min, d_max, n_shells);
    for (auto peak : found->getPeakList())
        resolution_shell.addPeak(peak);
    for (auto peak : predicted->getPeakList())
        resolution_shell.addPeak(peak);

    for (int i = n_shells - 1; i >= 0; --i) {
        double d_lower = resolution_shell.shell(i).dmin;
        double d_upper = resolution_shell.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(spacegroup, friedel);

        for (auto peak : resolution_shell.shell(i).peaks)
            merged_data_per_shell.addPeak(peak);

        ShellQuality quality;
        quality.computeQuality(merged_data_per_shell, d_lower, d_upper);
        resolution.push_back(quality);
    }
}

} // namespace nsx
