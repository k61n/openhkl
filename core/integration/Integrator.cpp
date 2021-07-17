//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/Integrator.cpp
//! @brief     Handles integrators for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/Integrator.h"

#include "base/utils/Logger.h"
#include "core/experiment/DataHandler.h"
#include "core/experiment/PeakFinder.h"
#include "core/integration/GaussianIntegrator.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/ShapeCollection.h"

namespace nsx {

Integrator::Integrator(std::shared_ptr<DataHandler> data_handler) : _data_handler(data_handler)
{
    _integrator_map.clear();
    _integrator_map.insert(
        std::make_pair(IntegratorType::PixelSum, std::make_unique<PixelSumIntegrator>(true, true)));
    _integrator_map.insert(
        std::make_pair(IntegratorType::Gaussian, std::make_unique<GaussianIntegrator>(true, true)));
    _integrator_map.insert(
        std::make_pair(IntegratorType::ISigma, std::make_unique<ISigmaIntegrator>()));
    _integrator_map.insert(
        std::make_pair(IntegratorType::Profile1D, std::make_unique<Profile1DIntegrator>()));
    _integrator_map.insert(
        std::make_pair(IntegratorType::Profile3D, std::make_unique<Profile3DIntegrator>()));
    _params = std::make_unique<IntegrationParameters>();
}

IntegratorMap* Integrator::getIntegratorMap()
{
    return &_integrator_map;
}

DataHandler* Integrator::getDataHandler()
{
    return _data_handler.get();
}

IPeakIntegrator* Integrator::getIntegrator(const IntegratorType integrator_type) const
{
    std::map<IntegratorType, std::unique_ptr<IPeakIntegrator>>::const_iterator it;
    for (it = _integrator_map.begin(); it != _integrator_map.end(); ++it) {
        if (it->first == integrator_type)
            return it->second.get();
    }
    return nullptr;
}

void Integrator::integratePeaks(
    IntegratorType integrator_type, sptrDataSet data, PeakCollection* peaks)
{
    nsxlog(
        Level::Info, "Integrator::integratePeaks: integrating PeakCollection",
        peaks->name());
    IPeakIntegrator* integrator = getIntegrator(integrator_type);
    integrator->setNNumors(1);
    integrator->integrate(peaks->getPeakList(), peaks->shapeCollection(), data, 1);
}

void Integrator::integratePeaks(
    sptrDataSet data, PeakCollection* peaks, IntegrationParameters* params, ShapeCollection* shapes)
{
    nsxlog(
        Level::Info, "Integrator::integratePeaks: integrating PeakCollection ",
        peaks->name());
    params->log(Level::Info);
    IPeakIntegrator* integrator = getIntegrator(_params->integrator_type);
    integrator->setParameters(*params);
    integrator->setNNumors(1);
    integrator->integrate(peaks->getPeakList(), shapes, data, 1);
}

void Integrator::integrateFoundPeaks(PeakFinder* peak_finder)
{
    nsxlog(Level::Info, "Integrator::integrateFoundPeaks");
    const DataMap* data = _data_handler->getDataMap();
    IPeakIntegrator* integrator = getIntegrator(IntegratorType::PixelSum);
    integrator->setNNumors(data->size());

    int n_numor = 1;
    for (const sptrDataSet& data : peak_finder->currentData()) {
        integrator->integrate(peak_finder->currentPeaks(), nullptr, data, n_numor);
        ++n_numor;
    }
}

void Integrator::integrateShapeCollection(
    std::vector<Peak3D*>& fit_peaks, sptrDataSet data, ShapeCollection* shape_collection,
    const AABB& aabb, const ShapeCollectionParameters& params)
{
    nsxlog(Level::Info, "Integrator::integrateShapeCollection");
    ShapeIntegrator integrator{
        shape_collection, aabb, params.nbins_x, params.nbins_y, params.nbins_z};
    integrator.setNNumors(1);
    integrator.setParameters(params);
    integrator.integrate(fit_peaks, shape_collection, data, 1);
}

void Integrator::setParameters(std::shared_ptr<IntegrationParameters> params)
{
    _params = params;
}

IntegrationParameters* Integrator::parameters()
{
    return _params.get();
}

} // namespace nsx
