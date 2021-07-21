//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/ShapeCollection.cpp
//! @brief     Implements classes PeakInterpolation, ShapeCollection
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/shape/Predictor.h"

#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/peak/Peak3D.h"
#include "core/peak/Qs2Events.h"
#include "core/shape/PeakCollection.h"

namespace nsx {

void PredictionParameters::log(const Level& level) const
{
    IntegrationParameters::log(level);
    nsxlog(level, "Peak prediction parameters: ");
    nsxlog(level, "d_min     = ", d_min);
    nsxlog(level, "d_max     = ", d_max);
}

Predictor::Predictor() {
    _params = std::make_unique<PredictionParameters>();
}

std::vector<Peak3D*> Predictor::buildPeaksFromMillerIndices(
    sptrDataSet data, const std::vector<MillerIndex>& hkls, const UnitCell* unit_cell,
    sptrProgressHandler handler)
{
    const Eigen::Matrix3d BU = unit_cell->reciprocalBasis();
    std::vector<ReciprocalVector> qs;
    for (const auto& idx : hkls)
        qs.emplace_back(idx.rowVector().cast<double>() * BU);
    nsxlog(
        Level::Info, "Predictor::buildPeaksFromMillerIndices: ", qs.size(),
        " q-vectors generated from ", hkls.size(), " Miller indices");

    const std::vector<DetectorEvent> events =
        algo::qVectorList2Events(qs, data->instrumentStates(), data->detector(), data->nFrames(),
                                 handler);

    std::vector<Peak3D*> peaks;
    for (auto event : events) {
        Peak3D* peak(new Peak3D(data));
        Eigen::Vector3d center = {event._px, event._py, event._frame};

        // dummy shape
        try {
            peak->setShape(Ellipsoid(center, 1.0));
            peak->setUnitCell(unit_cell);
            peaks.push_back(peak);
        } catch (...) {
            // invalid shape, nothing to do
        }
    }
    nsxlog(
        Level::Info, "Predictor::buildPeaksFromMillerIndices: ", peaks.size(),
        " peaks generated from ", hkls.size(), " Miller indices");
    return peaks;
}

void Predictor::predictPeaks(
    const sptrDataSet data, const UnitCell* unit_cell, sptrProgressHandler handler)
{
    _params->log(Level::Info);
    _predicted_peaks.clear();

    // Generate the Miller indices found in the [dmin,dmax] shell
    const auto& mono = data->reader()->diffractometer()->source().selectedMonochromator();

    const double wavelength = mono.wavelength();

    auto predicted_hkls =
        unit_cell->generateReflectionsInShell(_params->d_min, _params->d_max, wavelength);

    _predicted_peaks =
        buildPeaksFromMillerIndices(data, predicted_hkls, unit_cell, handler);
}

PredictionParameters* Predictor::parameters()
{
    return _params.get();
}

const std::vector<Peak3D*>& Predictor::peaks() const
{
    return _predicted_peaks;
}

} // namespace nsx
