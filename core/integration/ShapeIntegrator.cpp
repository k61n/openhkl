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
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/shape/Profile1D.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeModel.h"

namespace ohkl {

ShapeIntegrator::ShapeIntegrator() : PixelSumIntegrator()
{
}

void ShapeIntegrator::initialise(const AABB& aabb, ShapeModelParameters* params)
{
    _aabb = aabb;
    _nx = params->nbins_x;
    _ny = params->nbins_y;
    _nz = params->nbins_z;
    _nsubdiv = params->n_subdiv;
}

ComputeResult ShapeIntegrator::compute(
    Peak3D* peak, ShapeModel* shape_model, const IntegrationRegion& region)
{
    ComputeResult result;
    result.integrator_type = IntegratorType::Shape;

    const UnitCell* uc = peak->unitCell();
    auto data = peak->dataSet();

    if (!uc) {
        result.integration_flag = RejectionFlag::NoUnitCell;
        return result;
    }

    const ComputeResult pxsum_result = PixelSumIntegrator::compute(peak, shape_model, region);

    const double mean_bkg = pxsum_result.sum_background.value();
    const auto& events = region.peakData().events();
    const auto& counts = region.peakData().counts();

    Profile3D profile(_aabb, _nx, _ny, _nz);
    Profile1D integrated_profile(pxsum_result.sum_background, region.peakEnd());
    const PeakCoordinateSystem frame(peak);

    const Ellipsoid shape = peak->shape();

    for (size_t i = 0; i < events.size(); ++i) {
        const auto& ev = events[i];
        Eigen::Vector3d pos = ev.vector();
        const double dI = counts[i] - mean_bkg;
        // todo: variance here assumes Poisson (no gain or baseline)
        integrated_profile.addPoint(shape.r2(pos), counts[i]);

        if (shape_model->detectorCoords()) {
            pos -= peak->shape().center();
            if (_nsubdiv == 1)
                profile.addValue(pos, dI);
            else
                profile.addSubdividedValue(pos, dI, _nsubdiv);
        } else {
            if (_nsubdiv == 1)
                profile.addValue(frame.transform(ev), dI);
            else
                profile.addSubdividedValue(frame.transform(ev), dI, _nsubdiv);
        }
    }
    if (profile.normalize())
        shape_model->addPeak(peak, std::move(profile), std::move(integrated_profile));
    return result;
}

} // namespace ohkl
