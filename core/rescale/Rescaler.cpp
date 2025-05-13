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

#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/shape/PeakCollection.h"
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/RFactor.h"

#include <iostream>
#include <memory>

namespace ohkl {

std::vector<double> Rescaler::_minf = {};
int Rescaler::_niter = 0;

void RescalerParameters::log(const Level& level) const
{
    ohklLog(level, "Rescaler parameters:");
    ohklLog(level, "sum_intensity          = ", sum_intensity);
    ohklLog(level, "friedel                = ", friedel);
    ohklLog(level, "ftol                   = ", ftol);
    ohklLog(level, "xtol                   = ", xtol);
    ohklLog(level, "ctol                   = ", ctol);
    ohklLog(level, "max_iter               = ", max_iter);
    ohklLog(level, "init_step              = ", init_step);
    ohklLog(level, "frame_ratio            = ", frame_ratio);
}

    Rescaler::Rescaler(PeakCollection* collection, SpaceGroup group)
    : _peak_collection(collection), _space_group(group), _parameters(), _merged_peaks(nullptr)
{
    ohklLog(Level::Info, "Rescaler::Rescaler");

    _minf.clear();
    _niter = 0;

    int nframes = collection->data()->nFrames();
    double scale_upper = 1.0 + _parameters.frame_ratio;
    double scale_lower = 1.0 - _parameters.frame_ratio;
    for (std::size_t frame = 0; frame < nframes; ++frame) {
        _scale_factors.push_back(1.0);
        int idx = _scale_factors.size() - 1;
        if (frame == 0)
            // Constrain the first image in a data set to be 1.0
            _equality_constraints.push_back({idx, 1.0});
        else {
            // Constrain a scale factor to be within 100 * _frame_ratio% of the previous one
            _inequality_constraints.push_back({idx, 1.0, scale_upper});
            _inequality_constraints.push_back({idx, -1.0, -scale_lower});
        }
    }
}

double Rescaler::objective(const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    (void)grad;
    ohkl::Rescaler* rescaler = static_cast<ohkl::Rescaler*>(f_data);

    rescaler->updateScaleFactors(params);
    rescaler->merge();
    double rfactor = rescaler->rfactor();
    ++_niter;
    return rfactor;
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
        _space_group, collections, _parameters.friedel, _parameters.sum_intensity);
    // _merged_peaks->setDRange(1.5, 50.0);
}

double Rescaler::rfactor() const
{
    RFactor rfactor(_parameters.sum_intensity);
    rfactor.calculate(_merged_peaks.get());
    return rfactor.Rmerge();
}

double Rescaler::sumChi2() const
{
    double sum_chi2 = 0.0;
    for (const ohkl::MergedPeak& merged_peak : _merged_peaks->mergedPeakSet())
        sum_chi2 += merged_peak.chi2();
    return sum_chi2;
}

std::optional<double> Rescaler::rescale()
{
    ohklLog(Level::Info, "Rescaler::rescale");
    _parameters.log(Level::Info);
    MinimizerNLopt minimizer(_scale_factors.size(), objective, this);
    minimizer.setFTol(_parameters.ftol);
    minimizer.setXTol(_parameters.xtol);
    minimizer.setCTol(_parameters.ctol);
    minimizer.setMaxIter(_parameters.max_iter);
    minimizer.setInitStep(_parameters.init_step);
    for (auto& constraint : _equality_constraints)
        minimizer.addEqualityConstraint(equality_constraint, &constraint);
    for (auto& constraint : _inequality_constraints)
        minimizer.addInequalityConstraint(inequality_constraint, &constraint);
    std::optional<double> minf = minimizer.minimize(_scale_factors);

    return minf;
}

} // namespace ohkl
