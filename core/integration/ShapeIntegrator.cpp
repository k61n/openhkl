//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/ShapeIntegrator.cpp
//! @brief     Implements class ShapeIntegrator
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/ShapeIntegrator.h"
#include "base/geometry/Ellipsoid.h"
#include "core/data/DataSet.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/shape/Profile1D.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeModel.h"

namespace ohkl {

ShapeIntegrator::ShapeIntegrator(ShapeModel* lib, const AABB& aabb, int nx, int ny, int nz)
    : PixelSumIntegrator(false, false), _collection(lib), _aabb(aabb), _nx(nx), _ny(ny), _nz(nz)
{
}

bool ShapeIntegrator::compute(
    Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region)
{
    const UnitCell* uc = peak->unitCell();
    auto data = peak->dataSet();

    if (!uc) {
        peak->setIntegrationFlag(RejectionFlag::NoUnitCell);
        return false;
    }

    if (!data) {
        peak->setIntegrationFlag(RejectionFlag::NoDataSet);
        return false;
    }

    PixelSumIntegrator::compute(peak, shape_model, region);

    const double mean_bkg = _sumBackground.value();
    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    Profile3D profile(_aabb, _nx, _ny, _nz);
    // todo: don't use default constructor!
    Profile1D integrated_profile(_sumBackground, region.peakEnd());
    PeakCoordinateSystem frame(peak);

    Ellipsoid e = peak->shape();

    for (size_t i = 0; i < events.size(); ++i) {
        const auto& ev = events[i];
        Eigen::Vector3d x(ev.px, ev.py, ev.frame);
        const double dI = counts[i] - mean_bkg;
        // todo: variance here assumes Poisson (no gain or baseline)
        integrated_profile.addPoint(e.r2(x), counts[i]);

        if (_collection->detectorCoords()) {
            x -= peak->shape().center();
            profile.addValue(x, dI);
        } else {
            profile.addValue(frame.transform(ev), dI);
        }
    }
    if (profile.normalize())
        _collection->addPeak(peak, std::move(profile), std::move(integrated_profile));
    return true;
}

const ShapeModel* ShapeIntegrator::collection() const
{
    return _collection;
}

} // namespace ohkl
