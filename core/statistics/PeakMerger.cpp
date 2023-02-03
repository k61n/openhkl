//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/PeakMerger.cpp
//! @brief     Implements class PeakMerger
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/PeakMerger.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/shape/PeakCollection.h"
#include "tables/crystal/SpaceGroup.h"

#include <fstream>
#include <iomanip>

namespace ohkl {

void MergeParameters::log(const Level& level) const
{
    ohklLog(level, "Merge parameters:");
    ohklLog(level, "d_min                  = ", d_min);
    ohklLog(level, "d_max                  = ", d_max);
    ohklLog(level, "frame_min             = ", frame_min);
    ohklLog(level, "frame_max             = ", frame_max);
    ohklLog(level, "n_shells               = ", n_shells);
    ohklLog(level, "friedel                = ", friedel);
}

PeakMerger::PeakMerger(PeakCollection* peaks /* = nullptr */)
{
    _params = std::make_unique<MergeParameters>();
    if (peaks)
        addPeakCollection(peaks);
}

void PeakMerger::addPeakCollection(PeakCollection* peaks)
{
    _peak_collections.push_back(peaks);
}

void PeakMerger::setSpaceGroup(const SpaceGroup& group)
{
    _space_group = group;
}

void PeakMerger::reset()
{
    _peak_collections.clear();
    _merged_data.reset();
    _merged_data_per_shell.clear();
    _shell_qualities.clear();
    _overall_quality.clear();
}

void PeakMerger::mergePeaks()
{
    _merged_data.reset();
    _merged_data_per_shell.clear();
    ohklLog(Level::Info, "PeakMerger::mergePeaks: parameters");
    _params->log(Level::Info);
    _merged_data = std::make_unique<MergedData>(
        _space_group, _peak_collections, _params->friedel, _params->frame_min, _params->frame_max);
    ResolutionShell resolution_shell{_params->d_min, _params->d_max, _params->n_shells};

    // Sort the peaks by resolution shell (concentric shells in d)
    for (PeakCollection* collection : _peak_collections) {
        for (Peak3D* peak : collection->getPeakList())
            resolution_shell.addPeak(peak);
    }

    // Generate merged peak collections per resolution shell
    for (int i = _params->n_shells - 1; i >= 0; --i) {
        double d_lower = resolution_shell.shell(i).dmin;
        double d_upper = resolution_shell.shell(i).dmax;

        std::unique_ptr<MergedData> merged_data_per_shell = std::make_unique<MergedData>(
            _space_group, _params->friedel, _params->frame_min, _params->frame_max);
        merged_data_per_shell->setDRange(d_lower, d_upper);

        for (auto peak : resolution_shell.shell(i).peaks)
            merged_data_per_shell->addPeak(peak);

        _merged_data_per_shell.push_back(std::move(merged_data_per_shell));
    }
}

void PeakMerger::computeQuality()
{
    ShellQuality quality;
    _overall_quality.shells.clear();
    _shell_qualities.shells.clear();
    // R-factors, CC, completeness per shell
    for (auto&& merged_data : _merged_data_per_shell) {
        quality.computeQuality(
            *merged_data, merged_data->dMin(), merged_data->dMax(), _params->sum_intensity);
        _shell_qualities.addShell(quality);
    }
    // Overall R-factors, CC, completeness
    quality.computeQuality(*_merged_data, _params->d_min, _params->d_max, _params->sum_intensity);
    _overall_quality.addShell(quality);

    _overall_quality.log();
    _shell_qualities.log();
}

MergeParameters* PeakMerger::parameters() const
{
    return _params.get();
}

MergedData* PeakMerger::getMergedData() const
{
    return _merged_data.get();
}

std::vector<MergedData*> PeakMerger::getMergedDataPerShell() const
{
    std::vector<MergedData*> merged_data;
    for (auto&& item : _merged_data_per_shell)
        merged_data.push_back(item.get());
    return merged_data;
}

const DataResolution* PeakMerger::shellQuality()
{
    return &_shell_qualities;
}

const DataResolution* PeakMerger::overallQuality()
{
    return &_overall_quality;
}

std::string PeakMerger::summary()
{
    std::ostringstream oss;
    for (const auto& shell : _shell_qualities.shells)
        oss << shell.toString() << std::endl;
    return oss.str();
}

bool PeakMerger::saveStatistics(std::string filename)
{
    std::fstream file(filename, std::ios::out);
    if (!file.is_open())
        return false;

    file << std::fixed << std::setw(10) << "dmax" << std::fixed << std::setw(10) << "dmin"
         << std::fixed << std::setw(10) << "nobs" << std::fixed << std::setw(10) << "nmerge"
         << std::fixed << std::setw(11) << "Redundancy" << std::fixed << std::setw(10) << "R meas."
         << std::fixed << std::setw(12) << "R exp. meas." << std::fixed << std::setw(11)
         << "R merge" << std::fixed << std::setw(13) << "R exp. merge" << std::fixed
         << std::setw(10) << "Rpim" << std::fixed << std::setw(11) << "Rpim exp." << std::fixed
         << std::setw(10) << "CC half" << std::fixed << std::setw(10) << "CC star" << std::fixed
         << std::setw(10) << "compl." << std::endl;

    for (const auto& shell : _shell_qualities.shells) {
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
             << std::setprecision(3) << shell.CCstar << std::fixed << std::setw(10)
             << std::setprecision(3) << shell.Completeness << std::endl;
    }

    for (const auto& shell : _overall_quality.shells) {
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
             << std::setprecision(3) << shell.CCstar << std::fixed << std::setw(10)
             << std::setprecision(3) << shell.Completeness << std::endl;
    }

    file.close();
    return true;
}

} // namespace ohkl
