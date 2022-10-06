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

const std::map<PeakHistogramType, std::string> PeakStatistics::_histogram_strings {
    {PeakHistogramType::Intensity, "Intensity"},
    {PeakHistogramType::Sigma, "Sigma"},
    {PeakHistogramType::Strength, "Strength"}};

PeakStatistics::PeakStatistics() : _current_histogram(nullptr)
{
}

PeakStatistics::~PeakStatistics()
{
    _clearHistogram();
}

void PeakStatistics::setPeakCollection(PeakCollection* peaks, PeakHistogramType type)
{
    _peaks = peaks;
    _peak_data = _getPeakData(type);
    _max_value = *std::max_element(_peak_data.begin(), _peak_data.end());
    _min_value = *std::min_element(_peak_data.begin(), _peak_data.end());
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

std::vector<double> PeakStatistics::_getPeakData(PeakHistogramType type)
{
    _peak_data.clear();
    for (auto* peak : _peaks->getPeakList()) {
        switch (type) {
            case PeakHistogramType::Intensity:
                _peak_data.push_back(peak->correctedIntensity().value());
                break;
            case PeakHistogramType::Sigma:
                _peak_data.push_back(peak->correctedIntensity().sigma());
                break;
            case PeakHistogramType::Strength:
                _peak_data.push_back(peak->correctedIntensity().strength());
                break;
        }
    }
    return _peak_data;
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
    for (double point : _peak_data)
        gsl_histogram_increment(_current_histogram, point);
    return _current_histogram;
}

} // namespace ohkl
