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
#include "core/statistics/MergedPeakCollection.h"
#include "core/statistics/RFactor.h"

#include <memory>
#include <string>

namespace {
static const double eps = 1.0e-8;
}

namespace ohkl {

std::vector<double> Rescaler::_minf_history = {};
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
    ohklLog(level, "frame_ratio            = ", frame_ratio);
}

Rescaler::Rescaler()
    : _parameters()
    , _scale_factors()
    , _equality_constraints()
    , _inequality_constraints()
    , _merged_peaks(nullptr)
{
}

double Rescaler::objective(const std::vector<double>& params, std::vector<double>& grad, void* f_data)
{
    (void)grad;
    ohkl::Rescaler* rescaler = static_cast<ohkl::Rescaler*>(f_data);

    rescaler->updateScaleFactors(params);
    rescaler->merge();
    double rfactor = rescaler->rfactor();
    ++_niter;
    _minf_history.push_back(rfactor);
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
    // a * x_{n} <= b * x_{n-1} (human readable)
    // a * x_{n} - b * x_{n-1} <= 0 (form required by nlopt constraint function)
    (void)grad;
    ohkl::InequalityConstraintData* data = static_cast<ohkl::InequalityConstraintData*>(f_data);
    return (data->a * params.at(data->n)) - (data->b * params.at(data->n - 1));
}

void Rescaler::setPeakCollection(PeakCollection* collection, const SpaceGroup& group)
{
    _scale_factors.clear();
    _equality_constraints.clear();
    _inequality_constraints.clear();

    _space_group = group;
    _peak_collection = collection;

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

void Rescaler::updateScaleFactors(const std::vector<double>& parameters)
{
    ohklLog(Level::Debug, "Rescaler::updateScaleFactors");
    for (Peak3D* peak : _peak_collection->getPeakList()) {
        int frame = std::lround(peak->shape().center()[2]);
        double scale = parameters.at(frame);
        peak->setScale(scale);
    }
}

void Rescaler::merge()
{
    std::vector<PeakCollection*> collections = {_peak_collection};
    ohklLog(Level::Debug, "Rescaler::merge");
    _merged_peaks.reset();
    _merged_peaks = std::make_unique<MergedPeakCollection>(
        _space_group, collections, _parameters.friedel, _parameters.sum_intensity);
    // _merged_peaks->setDRange(1.5, 50.0);
}

double Rescaler::rfactor() const
{
    double Rmerge = 0.0;
    double I_total = 0.0;

    // go through each equivalence class of peaks
    for (const auto& peak : _merged_peaks->mergedPeakSet()) {
        const double n = double(peak.redundancy());

        // skip if there are fewer than two peaks
        if (n < 1.999)
            continue;

        const double Iave = peak.intensity().value();
        I_total += std::fabs(Iave) * peak.redundancy();
        for (const auto& p : peak.peaks()) {
            Intensity I;
            if (_parameters.sum_intensity)
                I = p->correctedSumIntensity();
            else
                I = p->correctedProfileIntensity();

            double diff = std::fabs(I.value() - Iave);
            Rmerge += diff;
        }
    }

    if (I_total < eps)
        Rmerge = 0.0;
    else
        Rmerge /= I_total;

    return Rmerge;
    // RFactor rfactor(_parameters.sum_intensity);
    // rfactor.calculate(_merged_peaks.get());
    // return rfactor.Rmerge();
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

    _niter = 0;
    _minf_history.clear();

    MinimizerNLopt minimizer(_scale_factors.size(), objective, this);
    minimizer.setFTol(_parameters.ftol);
    minimizer.setXTol(_parameters.xtol);
    minimizer.setCTol(_parameters.ctol);
    minimizer.setMaxIter(_parameters.max_iter);
    for (auto& constraint : _equality_constraints)
        minimizer.addEqualityConstraint(equality_constraint, &constraint);
    for (auto& constraint : _inequality_constraints)
        minimizer.addInequalityConstraint(inequality_constraint, &constraint);
    std::optional<double> minf = minimizer.minimize(_scale_factors);
    updateScaleFactors(_scale_factors);
    if (minf) {
        ohklLog(Level::Info, "Rescaler::rescale: initial objective = ", _minf_history.at(0));
        ohklLog(Level::Info, "Rescaler::rescale: final   objective = ", minf.value());
        ohklLog(Level::Info, "Rescaler::rescale: success. Scale factors:");
        for (std::size_t idx = 0; idx < _scale_factors.size(); ++idx) {
            std::string line =
                std::to_string(idx + 1) + " " + std::to_string(_scale_factors.at(idx));
            ohklLog(Level::Info, line);
        }
    }

    return minf;
}

} // namespace ohkl
