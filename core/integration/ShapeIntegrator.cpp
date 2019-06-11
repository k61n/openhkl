//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/integration/ShapeIntegrator.cpp
//! @brief     Implements class ShapeIntegrator
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/ShapeIntegrator.h"
#include "core/quantify/Intensity.h"
#include "tables/crystal/MillerIndex.h"
#include "core/experiment/DataSet.h"
#include "base/geometry/Ellipsoid.h"
#include "core/shape/Profile1D.h"
#include "core/peak/Peak3D.h"
#include "core/quantify/PeakCoordinateSystem.h"

namespace nsx {

ShapeIntegrator::ShapeIntegrator(sptrShapeLibrary lib, const AABB& aabb, int nx, int ny, int nz)
    : PixelSumIntegrator(false, false), _library(lib), _aabb(aabb), _nx(nx), _ny(ny), _nz(nz)
{
}

bool ShapeIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    auto uc = peak->unitCell();
    auto data = peak->data();

    if (!uc || !data)
        throw std::runtime_error("ShapeIntegrator: Peak must have unit cell and data attached");

    PixelSumIntegrator::compute(peak, region);

    const double mean_bkg = _meanBackground.value();
    const auto& events = region.data().events();
    const auto& counts = region.data().counts();

    Profile3D profile(_aabb, _nx, _ny, _nz);
    // todo: don't use default constructor!
    Profile1D integrated_profile(_meanBackground, region.peakEnd());
    PeakCoordinateSystem frame(peak);

    Ellipsoid e = peak->shape();

    for (size_t i = 0; i < events.size(); ++i) {
        const auto& ev = events[i];
        Eigen::Vector3d x(ev._px, ev._py, ev._frame);
        const double dI = counts[i] - mean_bkg;
        // todo: variance here assumes Poisson (no gain or baseline)
        integrated_profile.addPoint(e.r2(x), counts[i]);

        if (_library->detectorCoords()) {
            x -= peak->shape().center();
            profile.addValue(x, dI);
        } else {
            profile.addValue(frame.transform(ev), dI);
        }
    }
    if (profile.normalize())
        _library->addPeak(peak, std::move(profile), std::move(integrated_profile));
    return true;
}

sptrShapeLibrary ShapeIntegrator::library() const
{
    return _library;
}

} // namespace nsx
