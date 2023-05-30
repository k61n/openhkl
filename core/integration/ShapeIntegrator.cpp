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

ShapeIntegrator::ShapeIntegrator(
    ShapeModel* lib, const AABB& aabb, int nx, int ny, int nz, int subdiv)
    : PixelSumIntegrator(false, false)
    , _shape_model(lib)
    , _aabb(aabb)
    , _nx(nx)
    , _ny(ny)
    , _nz(nz)
    , _nsubdiv(subdiv)
{
}

bool ShapeIntegrator::compute(
    Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region)
{
    const UnitCell* uc = peak->unitCell();
    auto data = peak->dataSet();

    if (!uc) {
        peak->setIntegrationFlag(RejectionFlag::NoUnitCell, IntegratorType::PixelSum);
        return false;
    }

    if (!data) {
        peak->setIntegrationFlag(RejectionFlag::NoDataSet, IntegratorType::PixelSum);
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

        if (_shape_model->detectorCoords()) {
            x -= peak->shape().center();
            if (_nsubdiv == 1)
                profile.addValue(x, dI);
            else
                profile.addSubdividedValue(x, dI, _nsubdiv);
        } else {
            if (_nsubdiv == 1)
                profile.addValue(frame.transform(ev), dI);
            else
                profile.addSubdividedValue(x, dI, _nsubdiv);
        }
    }
    if (profile.normalize())
        _shape_model->addPeak(peak, std::move(profile), std::move(integrated_profile));
    return true;
}

const ShapeModel* ShapeIntegrator::shapeModel() const
{
    return _shape_model;
}

} // namespace ohkl
