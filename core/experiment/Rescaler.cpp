//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/Rescaler.cpp
//! @brief     Implements class Rescaler
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/Rescaler.h"

#include "base/fit/Minimizer.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/peak/Peak3D.h"
#include "core/statistics/MergedPeakCollection.h"

#include <Eigen/src/Core/Matrix.h>

#include <iostream>

namespace {

static const double eps = 1.0e-5;

//! Returns the matrix of parameter constraints.
Eigen::MatrixXd constraintKernel(int nparams, const std::vector<std::vector<int>>& constraints)
{
    std::vector<bool> is_free(nparams, true);
    std::vector<std::vector<double>> columns;

    // columns corresponding to the constrained parameters
    for (const std::vector<int>& constraint : constraints) {
        std::vector<double> column(nparams, 0.0);
        for (const int idx : constraint) {
            column[idx] = 1.0;
            is_free[idx] = false;
        }
        columns.push_back(column);
    }

    // columns corresponding to the free parameters
    for (int idx = 0; idx < nparams; ++idx) {
        if (!is_free[idx])
            continue;
        std::vector<double> column(nparams, 0.0);
        column[idx] = 1.0;
        columns.push_back(column);
    }

    // pack columns into a matrix
    Eigen::MatrixXd K(nparams, columns.size());
    for (size_t j = 0; j < columns.size(); ++j) {
        for (auto i = 0; i < nparams; ++i)
            K(i, j) = columns[j][i];
    }

    return K;

}

} // namespace

namespace ohkl {

Rescaler::Rescaler(sptrDataSet data, MergedPeakCollection* merged_peaks)
    : _data(data)
    , _merged_peaks(merged_peaks)
{
    std::size_t n_params = _data->nFrames();

    _scale_factors = Eigen::VectorXd::Ones(n_params);

    std::vector<int> ids;
    for (std::size_t idx = 0; idx < n_params; ++idx)
        ids.push_back(_params.addParameter(&_scale_factors[idx]));

    // _constraints.push_back(ids);
}

int Rescaler::residuals(Eigen::VectorXd& fvec)
{
    std::size_t count = -1;
    for (const auto& merged_peak : _merged_peaks->mergedPeakSet()) {
        bool sum_intensity = merged_peak.sumIntensity();

        // Compute the scaled merged intensity and scaled individual intensities
        std::vector<Intensity> intensities;
        std::size_t npeaks = 0;
        Intensity merged_intensity = {0, 0};
        for (const auto* peak : merged_peak.peaks()) {
            Intensity intensity = peak->correctedSumIntensity();
            if (!sum_intensity)
                intensity = peak->correctedProfileIntensity();

            long frame = std::lround(peak->shape().center()[2]);
            if (frame < 0) frame = 0;
            if (frame > _data->nFrames()) frame = _data->nFrames();

            const double scale = _scale_factors(frame);
            intensities.push_back(intensity * scale);
            merged_intensity += intensity * scale;
            ++npeaks;
        }
        merged_intensity = merged_intensity / npeaks;

        // residual for a merged peak is the chi squared of the *scaled* intensities
        double chi_sq = 0;
        for (std::size_t idx = 0; idx < merged_peak.peaks().size(); ++idx) {
            const double I = intensities[idx].value();
            const double std = intensities[idx].sigma();
            if (std > eps) {
                const double x = (I - merged_intensity.value()) / (std * std);
                chi_sq += x * x;
            }
        }

        fvec(++count) = chi_sq;
    }
    return 0;
}

bool Rescaler::optimize(unsigned int max_iter)
{
    Minimizer min;

    min.setxTol(1e-10);
    min.setfTol(1e-10);
    min.setgTol(1e-10);

    if (!_constraints.empty())
        _params.setKernel(constraintKernel(_params.nparams(), _constraints));

    min.initialize(_params, _merged_peaks->mergedPeakSet().size());
    min.set_f([&](Eigen::VectorXd& fvec) { return residuals(fvec); });
    return min.fit(max_iter);
}

} // namespace ohkl
