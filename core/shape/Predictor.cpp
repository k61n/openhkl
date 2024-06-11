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
#include "core/detector/Detector.h"
#include "core/gonio/Axis.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentStateSet.h"
#include "core/instrument/Monochromator.h"
#include "core/loader/IDataReader.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "tables/crystal/UnitCell.h"

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
    ohklLog(level, "delta_chi   = ", delta_chi);
    ohklLog(level, "delta_omega = ", delta_omega);
    ohklLog(level, "delta_phi   = ", delta_phi);
    ohklLog(level, "gamma       = ", gamma);
    ohklLog(level, "nu          = ", nu);
    ohklLog(level, "nframes     = ", nframes);
    ohklLog(level, "friedel     = ", friedel);
}

Predictor::Predictor() : _handler(nullptr)
{
    _params = std::make_unique<PredictionParameters>();
    _strategy_params = std::make_unique<StrategyParameters>();
}

Predictor::~Predictor()
{
    for (ohkl::Peak3D* peak : _predicted_peaks)
        delete peak;
    _predicted_peaks.clear();
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
    for (ohkl::Peak3D* peak : _predicted_peaks)
        delete peak;
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
    DataReaderParameters params;
    params.delta_omega = _strategy_params->delta_omega;
    params.delta_chi = _strategy_params->delta_chi;
    params.delta_phi = _strategy_params->delta_phi;
    params.twotheta_gamma = _strategy_params->gamma;
    params.twotheta_nu = _strategy_params->nu;

    for (std::size_t idx = 0; idx < _strategy_params->nframes; ++idx) {
        _strategy_diffractometer->addSampleAngles(idx, params);
        _strategy_diffractometer->addDetectorAngles(params);
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
