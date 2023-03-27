//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/ShapeModel.cpp
//! @brief     Implements classes PeakInterpolation, ShapeModel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/Predictor.h"

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Logger.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Units.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/integration/IIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "tables/crystal/MillerIndex.h"

#include <stdexcept>

namespace ohkl {

void PredictionParameters::log(const Level& level) const
{
    IntegrationParameters::log(level);
    ohklLog(level, "Peak prediction parameters: ");
    ohklLog(level, "d_min     = ", d_min);
    ohklLog(level, "d_max     = ", d_max);
}

void StrategyParameters::log(const Level& level) const
{
    PredictionParameters::log(level);
    ohklLog(level, "Strategy parameters: ");
    ohklLog(level, "delta_chi = ", delta_chi);
    ohklLog(level, "delta_omega = ", delta_omega);
    ohklLog(level, "delta_phi = ", delta_phi);
    ohklLog(level, "nframes = ", nframes);
    ohklLog(level, "friedel = ", friedel);
}

Predictor::Predictor() : _handler(nullptr)
{
    _params = std::make_unique<PredictionParameters>();
    _strategy_params = std::make_unique<StrategyParameters>();
}

std::vector<Peak3D*> Predictor::buildPeaksFromMillerIndices(
    sptrDataSet data, const std::vector<MillerIndex>& hkls, const sptrUnitCell unit_cell,
    sptrProgressHandler handler)
{
    const Eigen::Matrix3d BU = unit_cell->reciprocalBasis();
    std::vector<std::pair<MillerIndex, ReciprocalVector>> qs;
    for (const auto& idx : hkls) {
        ReciprocalVector q(idx.rowVector().cast<double>() * BU);
        qs.push_back({idx, q});
    }
    ohklLog(
        Level::Info, "Predictor::buildPeaksFromMillerIndices: ", qs.size(),
        " q-vectors generated from ", hkls.size(), " Miller indices");

    const std::vector<std::pair<MillerIndex, DetectorEvent>> events =
        algo::qMap2Events(qs, data->instrumentStates(), data->detector(), data->nFrames(), handler);

    std::vector<Peak3D*> peaks;
    int n_enabled = 0;
    for (const auto& [hkl, event] : events) {
        Peak3D* peak(new Peak3D(data, hkl));
        Eigen::Vector3d center = {event.px, event.py, event.frame};

        // setShape may disable the peak if the centre is out of bounds
        peak->setShape(Ellipsoid(center, 1.0));
        peak->setUnitCell(unit_cell);
        peaks.push_back(peak);

        // Check whether extinct from space group
        if (unit_cell->spaceGroup().isExtinct(hkl))
            peak->setRejectionFlag(RejectionFlag::Extinct);

        if (peak->enabled())
            ++n_enabled;
    }
    ohklLog(
        Level::Info, "Predictor::buildPeaksFromMillerIndices: ", n_enabled, "/", peaks.size(),
        " within detector bounds from ", hkls.size(), " Miller indices");
    return peaks;
}

void Predictor::predictPeaks(const sptrDataSet data, const sptrUnitCell unit_cell)
{
    _params->log(Level::Info);
    _predicted_peaks.clear();

    // Generate the Miller indices found in the [dmin,dmax] shell
    const auto& mono = data->diffractometer()->source().selectedMonochromator();

    const double wavelength = mono.wavelength();

    auto predicted_hkls =
        unit_cell->generateReflectionsInShell(_params->d_min, _params->d_max, wavelength);

    _predicted_peaks = buildPeaksFromMillerIndices(data, predicted_hkls, unit_cell, _handler);
}

void Predictor::strategyPredict(sptrDataSet data, const sptrUnitCell unit_cell)
{
    _strategy_params->log(Level::Info);

    _strategy_states.reset();
    _strategy_states = std::make_unique<InstrumentStateSet>(generateStates(data));
    data->setInstrumentStates(_strategy_states.get());

    _params->d_min = _strategy_params->d_min;
    _params->d_max = _strategy_params->d_max;

    predictPeaks(data, unit_cell);
}

InstrumentStateSet Predictor::generateStates(const sptrDataSet data)
{
    _strategy_diffractometer.reset(Diffractometer::create(data->diffractometer()->name()));
    const auto& detector_gonio = _strategy_diffractometer->detector()->gonio();
    const auto& sample_gonio = _strategy_diffractometer->sample().gonio();
    std::size_t n_detector_axes = detector_gonio.nAxes();
    std::size_t n_sample_axes = sample_gonio.nAxes();

    int omega_idx = -1, phi_idx = -1, chi_idx = -1;
    for (std::size_t idx = 0; idx < n_sample_axes; ++idx) {
        const std::string axis_name = sample_gonio.axis(idx).name();
        omega_idx = axis_name == ohkl::ax_omega ? int(idx) : omega_idx;
        chi_idx = axis_name == ohkl::ax_chi ? int(idx) : chi_idx;
        phi_idx = axis_name == ohkl::ax_phi ? int(idx) : phi_idx;
    }

    if (omega_idx == -1 || phi_idx == -1 || chi_idx == -1)
        throw std::runtime_error(
            "Predictor::generateStates: could not parse rotation axis indices");

    for (std::size_t idx = 0; idx < _strategy_params->nframes; ++idx) {
        std::vector<double> det_states(n_detector_axes);
        std::fill(det_states.begin(), det_states.end(), 0.0);
        _strategy_diffractometer->detectorStates.emplace_back(std::move(det_states));

        std::vector<double> sample_states(n_sample_axes);
        std::fill(sample_states.begin(), sample_states.end(), 0.0);
        sample_states[omega_idx] = idx * _strategy_params->delta_omega * deg;
        sample_states[phi_idx] = idx * _strategy_params->delta_phi * deg;
        sample_states[chi_idx] = idx * _strategy_params->delta_chi * deg;
        _strategy_diffractometer->sampleStates.emplace_back(std::move(sample_states));
    }

    std::string name = "strategy";
    return {_strategy_diffractometer.get(), name, _strategy_params->nframes};
}

PredictionParameters* Predictor::parameters()
{
    return _params.get();
}

StrategyParameters* Predictor::strategyParamters()
{
    return _strategy_params.get();
}

const std::vector<Peak3D*>& Predictor::peaks() const
{
    return _predicted_peaks;
}

unsigned int Predictor::numberOfPredictedPeaks()
{
    return _predicted_peaks.size();
}

void Predictor::setHandler(sptrProgressHandler handler)
{
    _handler = handler;
}

} // namespace ohkl
