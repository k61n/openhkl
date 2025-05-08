//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/rescale/Rescaler.cpp
//! @brief     Implements class Rescaler
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/rescale/Rescaler.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/shape/PeakCollection.h"

#include <iostream>
#include <memory>

namespace {

double objective(const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    (void)grad;
    ohkl::Rescaler* rescaler = static_cast<ohkl::Rescaler*>(f_data);

    for (std::size_t idx = 0; idx < params.size(); ++idx)
        std::cout << params[idx] << " ";
    std::cout << std::endl;

    rescaler->updateScaleFactors();
    rescaler->merge();

    double sum_chi2 = 0.0;
    for (const ohkl::MergedPeak& merged_peak : rescaler->mergedPeaks()->mergedPeakSet()) {
        if (merged_peak.redundancy() < 1.99)
            continue;
        double chi2 = merged_peak.chi2();
        std::cout << chi2 << " ";
        sum_chi2 += merged_peak.chi2();
    }
    std::cout << std::endl;

    std::cout << "sum_chi2 = " << sum_chi2 << std::endl;
    return sum_chi2;
}

double equality_constraint(
    const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    (void)grad;
    ohkl::EqualityConstraintData* data = static_cast<ohkl::EqualityConstraintData*>(f_data);
    return params.at(data->index) - data->value;
}

double inequality_constraint(
    const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    // Inequality of type
    // a * x_{n} <= b * x_{n-1}
    // a * x_{n} - b * x_{n-1} <= 0
    (void)grad;
    ohkl::InequalityConstraintData* data = static_cast<ohkl::InequalityConstraintData*>(f_data);
    return (data->a * params.at(data->n)) - (data->b * params.at(data->n - 1));
}
}

namespace ohkl {

Rescaler::Rescaler(
    std::vector<PeakCollection*> collections, SpaceGroup group, bool friedel, bool sum_intensity)
    : _peak_collections(collections)
    , _space_group(group)
    , _friedel(friedel)
    , _sum_intensity(sum_intensity)
    , _merged_peaks(nullptr)
    , _ftol(1.0e-3)
    , _ctol(1.0e-3)
    , _max_iter(2)
{
    ohklLog(Level::Info, "Rescaler::Rescaler");
    for (auto* peaks : _peak_collections) {
        int nframes = peaks->data()->nFrames();
        _scale_factors.insert_or_assign(peaks, std::vector<double*>(nframes));
        for (std::size_t frame = 0; frame < nframes; ++frame) {
            _parameters.push_back(1.0);
            int idx = _parameters.size() - 1;
            // Constrain the first image in a data set to be 1.0
            if (frame == 0)
                _equality_constraints.push_back({idx, 1.0});
            else {
                _inequality_constraints.push_back({idx, 1.0, 1.05});
                _inequality_constraints.push_back({idx, -1.0, -0.95});
            }
            _scale_factors.at(peaks).at(frame) = &_parameters.at(idx);
        }
    }
}

void Rescaler::updateScaleFactors()
{
    ohklLog(Level::Info, "Rescaler::updateScaleFactors");
    for (PeakCollection* collection : _peak_collections) {
        for (Peak3D* peak : collection->getPeakList()) {
            int frame = std::lround(peak->shape().center()[2]);
            double* scale = _scale_factors.at(collection).at(frame);
            peak->setScale(*scale);
        }
    }
}

void Rescaler::merge()
{
    ohklLog(Level::Info, "Rescaler::merge");
    _merged_peaks.reset();
    _merged_peaks = std::make_unique<MergedPeakCollection>(
        _space_group, _peak_collections, _friedel, _sum_intensity);
    // _merged_peaks->setDRange(1.5, 50.0);
}

std::optional<double> Rescaler::rescale()
{
    ohklLog(Level::Info, "Rescaler::rescale");
    MinimizerNLopt minimizer(_parameters.size(), objective, this);
    minimizer.setFTol(_ftol);
    minimizer.setCTol(_ctol);
    minimizer.setMaxIter(_max_iter);
    for (auto& constraint : _equality_constraints)
        minimizer.addEqualityConstraint(equality_constraint, &constraint);
    for (auto& constraint : _inequality_constraints)
        minimizer.addInequalityConstraint(inequality_constraint, &constraint);
    std::optional<double> minf = minimizer.minimize(_parameters);

    return minf;
}

} // namespace ohkl
