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
#include "base/utils/ProgressHandler.h"
#include "core/data/DataSet.h"
#include "core/integration/IIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/ResolutionShell.h"
#include "core/statistics/RotationSlice.h"

#include <fstream>
#include <iomanip>
#include <iostream>

namespace ohkl {

void MergeParameters::log(const Level& level) const
{
    ohklLog(level, "Merge parameters:");
    ohklLog(level, "d_min                  = ", d_min);
    ohklLog(level, "d_max                  = ", d_max);
    ohklLog(level, "first_frame            = ", first_frame);
    ohklLog(level, "last_frame             = ", last_frame);
    ohklLog(level, "n_shells               = ", n_shells);
    ohklLog(level, "friedel                = ", friedel);
}

PeakMerger::PeakMerger(PeakCollection* peaks /* = nullptr */) : _handler(nullptr)
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
    _sum_merged_data.reset();
    _sum_merged_data_per_shell.clear();
    _profile_merged_data.reset();
    _profile_merged_data_per_shell.clear();
    _sum_shell_qualities.clear();
    _sum_overall_quality.clear();
    _profile_shell_qualities.clear();
    _profile_overall_quality.clear();
}

void PeakMerger::mergePeaks()
{
    ohklLog(Level::Info, "PeakMerger::mergePeaks: parameters");
    _params->log(Level::Info);

    _sum_merged_data.reset();
    _sum_merged_data_per_shell.clear();
    _profile_merged_data.reset();
    _profile_merged_data_per_shell.clear();

    _sum_merged_data = std::make_unique<MergedPeakCollection>(
        _space_group, _peak_collections, _params->friedel, true, _params->first_frame,
        _params->last_frame);
    _profile_merged_data = std::make_unique<MergedPeakCollection>(
        _space_group, _peak_collections, _params->friedel, false, _params->first_frame,
        _params->last_frame);

    _sum_merged_data->setDRange(_params->d_min, _params->d_max);
    _profile_merged_data->setDRange(_params->d_min, _params->d_max);
    ResolutionShell resolution_shell{_params->d_min, _params->d_max, _params->n_shells};

    // For computing maximum possible completeness
    MergedPeakCollection merged_all(_space_group, _peak_collections, _params->friedel);
    merged_all.setDRange(_params->d_min, _params->d_max);
    DataQuality quality;
    quality.computeQuality(merged_all, true);
    _max_completeness = quality.Completeness;

    // Sort the peaks by resolution shell (concentric shells in d)
    for (PeakCollection* collection : _peak_collections) {
        for (Peak3D* peak : collection->getPeakList())
            resolution_shell.addPeak(peak);
    }

    // Generate merged peak collections per resolution shell
    for (int i = _params->n_shells - 1; i >= 0; --i) {
        double d_lower = resolution_shell.shell(i).dmin;
        double d_upper = resolution_shell.shell(i).dmax;

        std::unique_ptr<MergedPeakCollection> sum_merged_data_per_shell =
            std::make_unique<MergedPeakCollection>(
                _space_group, _params->friedel, true, _params->first_frame, _params->last_frame);

        std::unique_ptr<MergedPeakCollection> profile_merged_data_per_shell =
            std::make_unique<MergedPeakCollection>(
                _space_group, _params->friedel, false, _params->first_frame, _params->last_frame);

        for (PeakCollection* collection : _peak_collections) {
            sum_merged_data_per_shell->addPeakCollection(collection);
            profile_merged_data_per_shell->addPeakCollection(collection);
        }
        sum_merged_data_per_shell->setDRange(d_lower, d_upper);
        profile_merged_data_per_shell->setDRange(d_lower, d_upper);

        for (auto peak : resolution_shell.shell(i).peaks) {
            sum_merged_data_per_shell->addPeak(peak);
            profile_merged_data_per_shell->addPeak(peak);
        }

        _sum_merged_data_per_shell.push_back(std::move(sum_merged_data_per_shell));
        _profile_merged_data_per_shell.push_back(std::move(profile_merged_data_per_shell));
    }
}

std::vector<double> PeakMerger::strategyMerge(double fmin, double fmax, std::size_t nslices)
{
    std::vector<double> completeness_per_slice;
    RotationSlice slices(fmin, fmax, nslices);
    for (auto* collection : _peak_collections) {
        for (auto* peak : collection->getPeakList())
            slices.addPeak(peak);
    }
    if (_handler) {
        _handler->setStatus(
            ("Merging peaks over " + std::to_string(nslices) + " angle increments").c_str());
        _handler->setProgress(0);
    }

    for (std::size_t idx = 0; idx < slices.nslices(); ++idx) {
        _sum_merged_data.reset();
        _sum_merged_data =
            std::make_unique<MergedPeakCollection>(_space_group, _params->friedel, true);
        _sum_merged_data->setDRange(
            _params->d_min, _params->d_max, _peak_collections[0]->data(),
            _peak_collections[0]->unitCell());

        // Sort the peaks by resolution shell (concentric shells in d)
        for (Peak3D* peak : slices.slice(idx).peaks)
            _sum_merged_data->addPeak(peak);

        ShellQuality quality;
        quality.computeQuality(*_sum_merged_data, _params->d_min, _params->d_max);
        completeness_per_slice.push_back(quality.Completeness);
        if (_handler) {
            double progress = 100.0 * static_cast<double>(idx) / static_cast<double>(nslices);
            _handler->setProgress(progress);
        }
    }

    if (_handler) {
        _handler->setStatus("Merging complete");
        _handler->setProgress(100);
    }
    return completeness_per_slice;
}

void PeakMerger::computeQuality()
{
    ShellQuality quality;
    _sum_overall_quality.clear();
    _sum_shell_qualities.clear();
    _profile_overall_quality.clear();
    _profile_shell_qualities.clear();
    // R-factors, CC, completeness per shell
    for (std::size_t idx = 0; idx < _params->n_shells; ++idx) {
        quality.computeQuality(
            *_sum_merged_data_per_shell[idx], _sum_merged_data_per_shell[idx]->dMin(),
            _sum_merged_data_per_shell[idx]->dMax(), true);
        _sum_shell_qualities.addShell(quality);
        quality.computeQuality(
            *_profile_merged_data_per_shell[idx], _profile_merged_data_per_shell[idx]->dMin(),
            _profile_merged_data_per_shell[idx]->dMax(), false);
        _profile_shell_qualities.addShell(quality);
    }
    // Overall R-factors, CC, completeness
    quality.computeQuality(*_sum_merged_data, _params->d_min, _params->d_max, true);
    _sum_overall_quality.addShell(quality);
    quality.computeQuality(*_profile_merged_data, _params->d_min, _params->d_max, false);
    _profile_overall_quality.addShell(quality);

    _sum_overall_quality.log();
    _sum_shell_qualities.log();
    _profile_overall_quality.log();
    _profile_shell_qualities.log();
}

MergeParameters* PeakMerger::parameters() const
{
    return _params.get();
}

MergedPeakCollection* PeakMerger::sumMergedPeakCollection() const
{
    return _sum_merged_data.get();
}

std::vector<MergedPeakCollection*> PeakMerger::sumMergedPeakCollectionPerShell() const
{
    std::vector<MergedPeakCollection*> merged_data;
    for (auto&& item : _sum_merged_data_per_shell)
        merged_data.push_back(item.get());
    return merged_data;
}

MergedPeakCollection* PeakMerger::profileMergedPeakCollection() const
{
    return _profile_merged_data.get();
}

std::vector<MergedPeakCollection*> PeakMerger::profileMergedPeakCollectionPerShell() const
{
    std::vector<MergedPeakCollection*> merged_data;
    for (auto&& item : _profile_merged_data_per_shell)
        merged_data.push_back(item.get());
    return merged_data;
}

DataResolution* PeakMerger::sumShellQuality()
{
    return &_sum_shell_qualities;
}

DataResolution* PeakMerger::sumOverallQuality()
{
    return &_sum_overall_quality;
}

DataResolution* PeakMerger::profileShellQuality()
{
    return &_profile_shell_qualities;
}

DataResolution* PeakMerger::profileOverallQuality()
{
    return &_profile_overall_quality;
}

std::string PeakMerger::summary()
{
    std::ostringstream oss;
    oss << "Sum integration" << std::endl;
    oss << std::setw(8) << "dmin" << std::setw(8) << "dmax" << std::setw(8) << "Rmea"
        << std::setw(8) << "eRmea" << std::setw(8) << "Rmer" << std::setw(8) << "eRmer"
        << std::setw(8) << "Rpim" << std::setw(8) << "eRpim" << std::setw(8) << "CChalf"
        << std::setw(8) << "CCstar" << std::setw(8) << "Compl." << std::endl;
    for (const auto& shell : _sum_shell_qualities.shells)
        oss << shell.toString() << std::endl;
    oss << "Profile integration" << std::endl;
    oss << std::setw(8) << "dmin" << std::setw(8) << "dmax" << std::setw(8) << "Rmea"
        << std::setw(8) << "eRmea" << std::setw(8) << "Rmer" << std::setw(8) << "eRmer"
        << std::setw(8) << "Rpim" << std::setw(8) << "eRpim" << std::setw(8) << "CChalf"
        << std::setw(8) << "CCstar" << std::setw(8) << "Compl." << std::endl;
    for (const auto& shell : _profile_shell_qualities.shells)
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

    for (const auto& shell : _sum_shell_qualities.shells) {
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

    for (const auto& shell : _sum_overall_quality.shells) {
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

bool PeakMerger::savePeaks(std::string filename, bool sum_intensities) const
{
    std::string data;
    if (sum_intensities)
        data = _sum_merged_data->toStringUnmerged();
    else
        data = _profile_merged_data->toStringUnmerged();

    std::fstream file(filename, std::ios::out);
    if (!file.is_open())
        return false;

    file << data;
    file.close();
    return true;
}

std::vector<double> PeakMerger::getFigureOfMerit(FigureOfMerit fom, IntegratorType integrator)
{
    DataResolution* resolution;
    if (integrator == IntegratorType::PixelSum)
        resolution = &_sum_shell_qualities;
    else
        resolution = &_profile_shell_qualities;

    std::vector<double> result;
    for (const auto& shell : resolution->shells) {
        switch (fom) {
            case FigureOfMerit::d: result.push_back(shell.dmin); break;
            case FigureOfMerit::Rmerge: result.push_back(shell.Rmerge); break;
            case FigureOfMerit::Rmeas: result.push_back(shell.Rmeas); break;
            case FigureOfMerit::Rpim: result.push_back(shell.Rpim); break;
            case FigureOfMerit::CChalf: result.push_back(shell.CChalf); break;
            case FigureOfMerit::CCstar: result.push_back(shell.CCstar); break;
            case FigureOfMerit::Completeness: result.push_back(shell.Completeness); break;
        }
    }

    return result;
}

} // namespace ohkl
