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
#include "core/integration/IIntegrator.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/shape/Profile.h"
#include "core/shape/Profile1D.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeModel.h"

namespace ohkl {

const double ShapeIntegrator::_eps = 1.0e-10;

ShapeIntegrator::ShapeIntegrator() : PixelSumIntegrator() { }

void ShapeIntegrator::initialise(
    const AABB& aabb, ShapeModelParameters* params, ShapeModel* shapes)
{
    _aabb = aabb;
    _nx = params->nbins_x;
    _ny = params->nbins_y;
    _nz = params->nbins_z;
    _nsubdiv = params->n_subdiv;
    _kabsch = params->kabsch_coords;
    _shapes = shapes;
}

ComputeResult ShapeIntegrator::compute(
    Peak3D* peak, Profile* profile, const IntegrationRegion& region)
{
    ComputeResult result;
    result.integrator_type = IntegratorType::Shape;

    const UnitCell* uc = peak->unitCell();
    auto data = peak->dataSet();

    if (!uc) {
        result.integration_flag = RejectionFlag::NoUnitCell;
        return result;
    }

    const ComputeResult pxsum_result = PixelSumIntegrator::compute(peak, profile, region);

    const double mean_bkg = pxsum_result.sum_background.value();
    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    // Scale AABB according to peak shape for fixed ellipsoids
    double peak_scale = _params.peak_end;
    if (_params.region_type == RegionType::FixedEllipsoid) {
        const double r = peak->shape().radii().sum() / 3.0;
        if (!std::isnan(r) && std::abs(r) > _eps)
            peak_scale = _params.fixed_peak_end / r;
        else {
            result.integration_flag = RejectionFlag::InvalidShape;
            return result;
        }
    }

    AABB aabb = _aabb;
    if (_kabsch)
        aabb.rescale(peak_scale);

    Profile3D profile3d(aabb, _nx, _ny, _nz, _kabsch);
    Profile1D profile1d(pxsum_result.sum_background, peak_scale);
    const PeakCoordinateSystem frame(peak);

    const Ellipsoid shape = peak->shape();

    for (size_t i = 0; i < events.size(); ++i) {
        const auto& ev = events[i];
        Eigen::Vector3d pos = ev.vector();
        const double dI = counts[i] - mean_bkg;
        // todo: variance here assumes Poisson (no gain or baseline)
        profile1d.addPoint(shape.r2(pos), counts[i]);

        if (_shapes->detectorCoords()) {
            pos -= peak->shape().center();
            if (_nsubdiv == 1)
                profile3d.addValue(pos, dI);
            else
                profile3d.addSubdividedValue(pos, dI, _nsubdiv);
        } else {
            if (_nsubdiv == 1)
                profile3d.addValue(frame.transform(ev), dI);
            else
                profile3d.addSubdividedValue(frame.transform(ev), dI, _nsubdiv);
        }
    }
    if (profile3d.normalize())
        _shapes->addPeak(peak, std::move(profile3d), std::move(profile1d));
    return result;
}

} // namespace ohkl
