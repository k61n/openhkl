//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/ShapeModelBuilder.cpp
//! @brief     Handles ShapeModel construction for Experiment object
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/ShapeModelBuilder.h"

#include "base/utils/Logger.h"
#include "core/integration/IIntegrator.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/shape/PeakCollection.h"

namespace ohkl {

ShapeModelBuilder::ShapeModelBuilder() : _handler(nullptr)
{
    _params = std::make_unique<ShapeModelParameters>();
}

ShapeModel ShapeModelBuilder::integrate(
    std::vector<Peak3D*> peaks, const sptrDataSet data, bool thread_parallel)
{
    ohklLog(Level::Info, "ShapeModelBuilder::integrate: integrating ", peaks.size(), " peaks");
    ShapeModel shapes(*_params, data);

    ShapeIntegrator integrator;
    integrator.initialise(getAABB(), _params.get());

    IntegrationParameters int_params;
    int_params.region_type = _params->region_type;
    int_params.peak_end = _params->peak_end;
    int_params.bkg_begin = _params->bkg_begin;
    int_params.bkg_end = _params->bkg_end;
    int_params.fixed_peak_end = _params->fixed_peak_end;
    int_params.fixed_bkg_begin = _params->fixed_bkg_begin;
    int_params.fixed_bkg_end = _params->fixed_bkg_end;
    integrator.setParallel(thread_parallel);
    if (_handler)
        integrator.setHandler(_handler);
    integrator.setParameters(int_params);
    integrator.parallelIntegrate(peaks, &shapes, data);
    ohklLog(Level::Info, "ShapeModelBuilder::integrate: finished integrating shapes");
    return shapes;
}

ShapeModel ShapeModelBuilder::build(PeakCollection* peaks, const sptrDataSet data, bool thread_parallel)
{
    ohklLog(Level::Info, "ShapeModelBuilder::build: building shape model");
    peaks->computeSigmas();
    _params->sigma_d = peaks->sigmaD();
    _params->sigma_m = peaks->sigmaM();
    std::vector<ohkl::Peak3D*> fit_peaks;

    for (ohkl::Peak3D* peak : peaks->getPeakList()) {
        if (!peak->enabled())
            continue;
        const double d = 1.0 / peak->q().rowVector().norm();

        if (d > _params->d_max || d < _params->d_min)
            continue;

        const ohkl::Intensity intensity = peak->correctedSumIntensity();

        if (intensity.value() <= _params->strength_min * intensity.sigma())
            continue;
        fit_peaks.push_back(peak);
    }

    // ohklLog(Level::Info, "ShapeModel::build: updating fit");
    // updateFit(1000);
    ShapeModel shapes = integrate(fit_peaks, data, thread_parallel);
    ohklLog(Level::Info, "ShapeModelBuilder::build: done");
    return shapes;
}

AABB ShapeModelBuilder::getAABB()
{
    AABB aabb;
    if (_params->kabsch_coords) {
        const Eigen::Vector3d sigma(_params->sigma_d, _params->sigma_d, _params->sigma_m);
        aabb.setLower(-_params->peak_end * sigma);
        aabb.setUpper(_params->peak_end * sigma);
    } else {
        const Eigen::Vector3d dx(_params->nbins_x, _params->nbins_y, _params->nbins_z);
        aabb.setLower(-0.5 * dx);
        aabb.setUpper(0.5 * dx);
    }
    return aabb;
}


} // namespace ohkl
