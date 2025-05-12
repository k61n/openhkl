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

namespace ohkl {

std::vector<double> Rescaler::_minf = {};
int Rescaler::_niter = 0;

Rescaler::Rescaler(PeakCollection* collection, SpaceGroup group, bool friedel, bool sum_intensity)
    : _peak_collection(collection)
    , _space_group(group)
    , _friedel(friedel)
    , _sum_intensity(sum_intensity)
    , _merged_peaks(nullptr)
    , _ftol(1.0e-3)
    , _ctol(1.0e-3)
    , _max_iter(1e5)
{
    ohklLog(Level::Info, "Rescaler::Rescaler");

    _minf.clear();
    _niter = 0;

    int nframes = collection->data()->nFrames();
    for (std::size_t frame = 0; frame < nframes; ++frame) {
        _parameters.push_back(1.0);
        int idx = _parameters.size() - 1;
        if (frame == 0)
            // Constrain the first image in a data set to be 1.0
            _equality_constraints.push_back({idx, 1.0});
        else {
            // Constrain a scale factor to be within 5% of the previous one
            _inequality_constraints.push_back({idx, 1.0, 1.05});
            _inequality_constraints.push_back({idx, -1.0, -0.95});
        }
    }
}

double Rescaler::objective(const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    (void)grad;
    ohkl::Rescaler* rescaler = static_cast<ohkl::Rescaler*>(f_data);

    rescaler->updateScaleFactors(params);
    rescaler->merge();

    double sum_chi2 = 0.0;
    for (const ohkl::MergedPeak& merged_peak : rescaler->mergedPeaks()->mergedPeakSet()) {
        double chi2 = merged_peak.chi2();
        _minf.push_back(chi2);
        sum_chi2 += chi2;
    }

    ++_niter;
    return sum_chi2;
}

double Rescaler::equality_constraint(
    const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    (void)grad;
    ohkl::EqualityConstraintData* data = static_cast<ohkl::EqualityConstraintData*>(f_data);
    return params.at(data->index) - data->value;
}

double Rescaler::inequality_constraint(
    const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    // Inequality of type
    // a * x_{n} <= b * x_{n-1}
    // a * x_{n} - b * x_{n-1} <= 0
    (void)grad;
    ohkl::InequalityConstraintData* data = static_cast<ohkl::InequalityConstraintData*>(f_data);
    return (data->a * params.at(data->n)) - (data->b * params.at(data->n - 1));
}

void Rescaler::updateScaleFactors(const std::vector<double>& parameters)
{
    ohklLog(Level::Info, "Rescaler::updateScaleFactors");
    for (Peak3D* peak : _peak_collection->getPeakList()) {
        int frame = std::lround(peak->shape().center()[2]);
        double scale = parameters.at(frame);
        peak->setScale(scale);
    }
}

void Rescaler::merge()
{
    std::vector<PeakCollection*> collections = {_peak_collection};
    ohklLog(Level::Info, "Rescaler::merge");
    _merged_peaks.reset();
    _merged_peaks = std::make_unique<MergedPeakCollection>(
        _space_group, collections, _friedel, _sum_intensity);
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
