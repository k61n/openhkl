//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/IntegrationHandler.cpp
//! @brief     Handles integrators for Experiment object
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/IntegrationHandler.h"
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

IntegrationHandler::~IntegrationHandler() = default;

IntegrationHandler::IntegrationHandler(std::shared_ptr<DataHandler> data_handler)
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
    _data_handler = data_handler;
}

IntegratorMap* IntegrationHandler::getIntegratorMap()
{
    return &_integrator_map;
}

DataHandler* IntegrationHandler::getDataHandler()
{
    return _data_handler.get();
}

IPeakIntegrator* IntegrationHandler::getIntegrator(const IntegratorType integrator_type) const
{
    std::map<IntegratorType, std::unique_ptr<IPeakIntegrator>>::const_iterator it;
    for (it = _integrator_map.begin(); it != _integrator_map.end(); ++it) {
        if (it->first == integrator_type)
            return it->second.get();
    }
    return nullptr;
}

void IntegrationHandler::integratePeaks(
    IntegratorType integrator_type, PeakCollection* peak_collection, double d_min, double d_max)
{
    nsxlog(
        Level::Info, "IntegrationHandler::integratePeaks: integrating PeakCollection",
        peak_collection);
    IPeakIntegrator* integrator = getIntegrator(integrator_type);

    nsx::PeakFilter filter;
    filter.resetFiltering(peak_collection);
    filter.setDRange(std::array<double, 2UL>{d_min, d_max});
    filter.filterDRange(peak_collection);
    std::vector<Peak3D*> peaks = peak_collection->getFilteredPeakList();

    const DataMap* data = _data_handler->getDataMap();
    integrator->setNNumors(data->size());
    int n_numor = 1;
    for (DataMap::const_iterator it = data->begin(); it != data->end(); ++it) {
        integrator->integrate(peaks, peak_collection->shapeCollection(), it->second, n_numor);
        ++n_numor;
    }
}

void IntegrationHandler::integratePeaks(
    IPeakIntegrator* integrator, PeakCollection* peaks, IntegrationParameters* params,
    ShapeCollection* shapes)
{
    nsxlog(Level::Info, "IntegrationHandler::integratePeaks: integrating PeakCollection ", peaks);
    const DataMap* data = _data_handler->getDataMap();
    integrator->setNNumors(data->size());
    integrator->setParameters(*params);
    int n_numor = 1;
    for (DataMap::const_iterator it = data->begin(); it != data->end(); ++it) {
        integrator->integrate(peaks->getPeakList(), shapes, it->second, n_numor);
        ++n_numor;
    }
}

void IntegrationHandler::integratePredictedPeaks(
    IntegratorType integrator_type, PeakCollection* peak_collection,
    ShapeCollection* shape_collection, PredictionParameters& params)
{
    nsxlog(Level::Info, "IntegrationHandler::integratePredictedPeaks");
    IPeakIntegrator* integrator = getIntegrator(integrator_type);
    integrator->setParameters(params);
    params.log(Level::Info);
    nsx::PeakFilter filter;
    filter.resetFiltering(peak_collection);
    filter.setDRange(std::array<double, 2UL>{params.d_min, params.d_max});
    filter.filterDRange(peak_collection);
    std::vector<Peak3D*> peaks = peak_collection->getFilteredPeakList();

    const DataMap* data = _data_handler->getDataMap();
    integrator->setNNumors(data->size());
    int n_numor = 1;
    for (DataMap::const_iterator it = data->begin(); it != data->end(); ++it) {
        integrator->integrate(peaks, shape_collection, it->second, n_numor);
        ++n_numor;
    }
}

void IntegrationHandler::integrateFoundPeaks(
    IntegratorType integrator_type, PeakFinder* peak_finder)
{
    nsxlog(Level::Info, "IntegrationHandler::integrateFoundPeaks");
    IPeakIntegrator* integrator = getIntegrator(integrator_type);
    const DataMap* data = _data_handler->getDataMap();
    integrator->setNNumors(data->size());

    int n_numor = 1;
    for (const sptrDataSet& data : peak_finder->currentData()) {
        integrator->integrate(peak_finder->currentPeaks(), nullptr, data, n_numor);
        ++n_numor;
    }
}

ShapeCollection& IntegrationHandler::integrateShapeCollection(
    std::vector<Peak3D*>& fit_peaks, ShapeCollection* shape_collection, const AABB& aabb,
    const ShapeCollectionParameters& params)
{
    nsxlog(Level::Info, "IntegrationHandler::integrateShapeCollection");
    ShapeIntegrator integrator{
        shape_collection, aabb, params.nbins_x, params.nbins_y, params.nbins_z};
    integrator.setNNumors(1);
    integrator.setParameters(params);

    const DataMap* data = _data_handler->getDataMap();
    integrator.setNNumors(data->size());
    // TODO: (zamaan) change numors to a argument of buildShapeCollection
    // Right now, there is no metadata for which DataSet was used to
    // Generate the peak collection
    int n_numor = 1;
    for (auto const& [key, data] : *data) {
        integrator.integrate(fit_peaks, shape_collection, data, n_numor);
        ++n_numor;
    }

    return *shape_collection;
}

} // namespace nsx
