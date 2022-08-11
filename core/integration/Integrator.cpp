//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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
#include "core/shape/ShapeModel.h"

namespace ohkl {

Integrator::Integrator(std::shared_ptr<DataHandler> data_handler)
    : _handler(nullptr), _data_handler(data_handler)
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
    ohklLog(
        Level::Info,
        "Integrator::integratePeaks: integrating PeakCollection '" + peaks->name() + "'");
    IPeakIntegrator* integrator = getIntegrator(integrator_type);
    integrator->setNNumors(1);
    integrator->integrate(peaks->getPeakList(), peaks->shapeModel(), data, 1);
    peaks->setIntegrated(true);

    _n_peaks = 0;
    _n_valid = 0;
    for (auto peak : peaks->getPeakList()) {
        ++_n_peaks;
        if (peak->enabled())
            ++_n_valid;
    }
}

void Integrator::integratePeaks(
    sptrDataSet data, PeakCollection* peaks, IntegrationParameters* params, ShapeModel* shapes)
{
    ohklLog(
        Level::Info,
        "Integrator::integratePeaks: integrating PeakCollection '" + peaks->name() + "'");
    params->log(Level::Info);
    IPeakIntegrator* integrator = getIntegrator(_params->integrator_type);
    integrator->setParameters(*params);
    integrator->setNNumors(1);
    integrator->integrate(peaks->getPeakList(), shapes, data, 1);
    peaks->setIntegrated(true);

    _n_peaks = 0;
    _n_valid = 0;
    for (auto peak : peaks->getPeakList()) {
        ++_n_peaks;
        if (peak->enabled())
            ++_n_valid;
    }
}

void Integrator::integrateFoundPeaks(PeakFinder* peak_finder)
{
    ohklLog(Level::Info, "Integrator::integrateFoundPeaks");
    const DataMap* data = _data_handler->getDataMap();
    IPeakIntegrator* integrator = getIntegrator(IntegratorType::PixelSum);
    integrator->setNNumors(data->size());

    int n_numor = 1;
    for (const sptrDataSet& data : peak_finder->currentData()) {
        integrator->integrate(peak_finder->currentPeaks(), nullptr, data, n_numor);
        ++n_numor;
    }

    _n_peaks = 0;
    _n_valid = 0;
    for (auto peak : peak_finder->currentPeaks()) {
        ++_n_peaks;
        if (peak->enabled())
            ++_n_valid;
    }
    // no further checks
    // peak_finder->getPeakCollection()->setIntegrated(true); // doesnt work since peak collection
    // does not exist yet
    peak_finder->setIntegrated(true);
}

void Integrator::integrateShapeModel(
    std::vector<Peak3D*> fit_peaks, sptrDataSet data, ShapeModel* shape_model, const AABB& aabb,
    const ShapeModelParameters& params)
{
    ohklLog(Level::Info, "Integrator::integrateShapeModel");
    ShapeIntegrator integrator{shape_model, aabb, params.nbins_x, params.nbins_y, params.nbins_z};
    integrator.setNNumors(1);
    if (_handler)
        integrator.setHandler(_handler);
    integrator.setParameters(params);
    integrator.integrate(fit_peaks, shape_model, data, 1);
}

void Integrator::setParameters(std::shared_ptr<IntegrationParameters> params)
{
    _params = params;
}

IntegrationParameters* Integrator::parameters()
{
    return _params.get();
}

void Integrator::setHandler(sptrProgressHandler handler)
{
    _handler = handler;
}

unsigned int Integrator::numberOfPeaks()
{
    return _n_peaks;
}

unsigned int Integrator::numberOfValidPeaks()
{
    return _n_valid;
}

} // namespace ohkl
