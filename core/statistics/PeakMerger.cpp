//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/statistics/PeakMerger.cpp
//! @brief     Implements class PeakMerger
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/PeakMerger.h"

#include "base/utils/Logger.h"
#include "core/shape/PeakCollection.h"
#include "tables/crystal/SpaceGroup.h"

namespace nsx {

void MergeParameters::log(const Level& level) const
{
    nsxlog(level, "Merge parameters:");
    nsxlog(level, "d_min                  = ", d_min);
    nsxlog(level, "d_max                  = ", d_max);
    nsxlog(level, "n_shells               = ", n_shells);
    nsxlog(level, "friedel                = ", friedel);
}

PeakMerger::PeakMerger(PeakCollection* peaks /* = nullptr */)
{
    if (peaks)
        _peak_collections.push_back(peaks);
}

void PeakMerger::addPeakCollection(PeakCollection* peaks)
{
    _peak_collections.push_back(peaks);
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
    nsxlog(Level::Info, "PeakMerger::mergePeaks: parameters");
    _params->log(Level::Info);
    _merged_data = std::make_unique<MergedData>(_peak_collections, _params->friedel);
    SpaceGroup space_group = _merged_data->spaceGroup();
    ResolutionShell resolution_shell{_params->d_min, _params->d_max, _params->n_shells};

    for (PeakCollection* collection : _peak_collections) {
        for (Peak3D* peak : collection->getPeakList())
            resolution_shell.addPeak(peak);
    }

    for (int i = _params->n_shells - 1; i >= 0; --i) {
        double d_lower = resolution_shell.shell(i).dmin;
        double d_upper = resolution_shell.shell(i).dmax;

        std::unique_ptr<MergedData> merged_data_per_shell =
            std::make_unique<MergedData>(space_group, _params->friedel);
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
    for (auto&& merged_data : _merged_data_per_shell) {
        quality.computeQuality(*merged_data, merged_data->dMin(), merged_data->dMax());
        quality.setCompleteness(merged_data->completeness());
        _shell_qualities.addShell(quality);
    }
    quality.computeQuality(*_merged_data, _params->d_min, _params->d_max);
    _overall_quality.addShell(quality);

    _overall_quality.log();
    _shell_qualities.log();
}

MergeParameters* PeakMerger::parameters() const
{
    return _params.get();
}

void PeakMerger::setParameters(std::shared_ptr<MergeParameters> params)
{
    _params = params;
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

} // namespace nsx
