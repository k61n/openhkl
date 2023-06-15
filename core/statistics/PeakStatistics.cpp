//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/PeakStatistics.cpp
//! @brief     Implements class PeakStatistics
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/PeakStatistics.h"

#include "core/shape/PeakCollection.h"
#include <algorithm>
#include <gsl/gsl_histogram.h>

namespace ohkl {

const std::map<PeakHistogramType, std::string> PeakStatistics::_histogram_strings{
    {PeakHistogramType::Intensity, "Intensity"},
    {PeakHistogramType::Sigma, "Sigma"},
    {PeakHistogramType::Strength, "Strength"},
    {PeakHistogramType::BkgGradient, "Background gradient"},
    {PeakHistogramType::BkgGradientSigma, "Background gradient sigma"}};

PeakStatistics::PeakStatistics() : _current_histogram(nullptr), _sum_intensities(true) { }

PeakStatistics::~PeakStatistics()
{
    _clearHistogram();
}

void PeakStatistics::setPeakCollection(PeakCollection* peaks, PeakHistogramType type)
{
    _peaks = peaks;
    _getPeakData(type);
    std::sort(_peak_data.rbegin(), _peak_data.rend()); // descending
    _min_value = _peak_data.back().first;
    _max_value = _peak_data.front().first;
    if (_max_value < _min_value + 1)
        _max_value = _min_value + 1;
}

void PeakStatistics::_initHistogram(std::size_t nbins)
{
    _nbins = nbins;
    _clearHistogram();
    _current_histogram = gsl_histogram_alloc(_nbins);
}

void PeakStatistics::_clearHistogram()
{
    if (_current_histogram == nullptr)
        return;
    gsl_histogram_free(_current_histogram);
    _current_histogram = nullptr;
}

void PeakStatistics::_getPeakData(PeakHistogramType type)
{
    _peak_data.clear();
    for (auto* peak : _peaks->getPeakList()) {
        if (!peak->enabled())
            continue;
        switch (type) {
            case PeakHistogramType::Intensity:
                _peak_data.push_back({intensity(peak).value(), peak});
                break;
            case PeakHistogramType::Sigma:
                _peak_data.push_back({intensity(peak).sigma(), peak});
                break;
            case PeakHistogramType::Strength:
                _peak_data.push_back({intensity(peak).strength(), peak});
                break;
            case PeakHistogramType::BkgGradient:
                _peak_data.push_back({peak->meanBkgGradient().value(), peak});
                break;
            case PeakHistogramType::BkgGradientSigma:
                _peak_data.push_back({peak->meanBkgGradient().sigma(), peak});
                break;
        }
    }
}

double PeakStatistics::maxCount() const
{
    return gsl_histogram_max_val(_current_histogram);
}

double PeakStatistics::minCount() const

{
    return gsl_histogram_min_val(_current_histogram);
}

gsl_histogram* PeakStatistics::computeHistogram(std::size_t nbins)
{
    _nbins = nbins;
    _initHistogram(nbins);
    gsl_histogram_set_ranges_uniform(_current_histogram, _min_value, _max_value);
    for (const auto& [point, peak] : _peak_data)
        gsl_histogram_increment(_current_histogram, point);
    return _current_histogram;
}

std::pair<double, double> PeakStatistics::computeStatistics()
{
    double sum = 0;
    double sum2 = 0;
    double npoints = _peak_data.size();
    for (const auto& [datum, peak] : _peak_data) {
        sum += datum;
        sum2 += datum * datum;
    }
    double mean = sum / npoints;
    double var = (sum2 - npoints * mean * mean) / (npoints - 1);
    return {mean, std::sqrt(var)};
}

std::vector<Peak3D*> PeakStatistics::findOutliers(double factor /* = 3.0 */)
{
    std::vector<Peak3D*> outliers;
    auto [mean, sigma] = computeStatistics();
    double data_max = mean + sigma * factor;
    for (std::size_t idx = 0; idx < _peak_data.size(); ++idx) {
        if (_peak_data.at(idx).first > data_max)
            outliers.push_back(_peak_data.at(idx).second);
        else
            break;
    }
    return outliers;
}

Intensity PeakStatistics::intensity(Peak3D* peak) const
{
    if (_sum_intensities)
        return peak->correctedSumIntensity();
    else
        return peak->correctedProfileIntensity();
}

} // namespace ohkl
