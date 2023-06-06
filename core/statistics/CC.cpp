//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/CC.cpp
//! @brief     Implements class CC
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/statistics/CC.h"

#include "core/statistics/MergedData.h"
#include "core/statistics/MergedPeak.h"

namespace ohkl {

CC::CC(bool sum_intensity) : _CChalf(0), _CCstar(0), _sum_intensity(sum_intensity) { };

void CC::calculate(MergedData* data)
{
    std::vector<MergedPeak> peaks;

    for (const auto& peak : data->mergedPeakSet())
        peaks.push_back(peak);
    calculate(peaks);
}

void CC::calculate(std::vector<MergedPeak> peaks)
{
    double xx, xy, yy, x, y;
    xx = xy = yy = x = y = 0.0;
    _nPeaks = 0;

    for (const auto& peak : peaks) {
        if (peak.redundancy() < 2)
            continue;

        auto split = peak.split(_sum_intensity);
        MergedPeak& p1 = split.first;
        MergedPeak& p2 = split.second;

        const double I1 = p1.intensity().value();
        const double I2 = p2.intensity().value();

        xx += I1 * I1;
        xy += I1 * I2;
        yy += I2 * I2;
        x += I1;
        y += I2;

        ++_nPeaks;
    }

    if (_nPeaks == 0) {
        _CChalf = _CCstar = 0.0;
        return;
    }

    const double numerator = xy - x * y / _nPeaks;
    const double varx = xx - x * x / _nPeaks;
    const double vary = yy - y * y / _nPeaks;

    _CChalf = numerator / std::sqrt(varx * vary);
    _CCstar = std::sqrt(2 * _CChalf / (1.0 + _CChalf));
}

double CC::CChalf() const
{
    return _CChalf;
}

double CC::CCstar() const
{
    return _CCstar;
}

unsigned int CC::nPeaks() const
{
    return _nPeaks;
}

} // namespace ohkl
