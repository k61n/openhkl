//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/ShapeModel.cpp
//! @brief     Implements classes PeakInterpolation, ShapeModel
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/Predictor.h"

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Logger.h"
#include "base/utils/ProgressHandler.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "tables/crystal/MillerIndex.h"

namespace ohkl {

void PredictionParameters::log(const Level& level) const
{
    IntegrationParameters::log(level);
    ohklLog(level, "Peak prediction parameters: ");
    ohklLog(level, "d_min     = ", d_min);
    ohklLog(level, "d_max     = ", d_max);
}

Predictor::Predictor() : _handler(nullptr)
{
    _params = std::make_unique<PredictionParameters>();
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
    for (const auto& [hkl, event] : events) {
        Peak3D* peak(new Peak3D(data, hkl));
        Eigen::Vector3d center = {event.px, event.py, event.frame};

        // setShape may disable the peak if the centre is out of bounds
        peak->setShape(Ellipsoid(center, 1.0));
        if (peak->selected()) {
            peak->setUnitCell(unit_cell);
            peaks.push_back(peak);
        }
    }
    ohklLog(
        Level::Info, "Predictor::buildPeaksFromMillerIndices: ", peaks.size(),
        " peaks generated from ", hkls.size(), " Miller indices");
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

PredictionParameters* Predictor::parameters()
{
    return _params.get();
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
