//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/Integrator.cpp
//! @brief     Handles integrators for Experiment object
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/Integrator.h"

#include "base/utils/Logger.h"
#include "core/experiment/DataHandler.h"
#include "core/experiment/PeakFinder.h"
#include "core/integration/IIntegrator.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/peak/Peak3D.h"
#include "core/shape/PeakCollection.h"
#include "core/shape/ShapeModel.h"

namespace ohkl {

Integrator::Integrator(std::shared_ptr<DataHandler> data_handler)
    : _handler(nullptr), _data_handler(data_handler), _factory()
{
    _params = std::make_unique<IntegrationParameters>();
}

DataHandler* Integrator::getDataHandler()
{
    return _data_handler.get();
}

IIntegrator* Integrator::getIntegrator(const IntegratorType integrator_type) const
{
    return _factory.create(integrator_type);
}

void Integrator::integratePeaks(
    sptrDataSet data, PeakCollection* peaks, IntegrationParameters* params, ShapeModel* shapes,
    bool parallel)
{
    ohklLog(
        Level::Info,
        "Integrator::integratePeaks: integrating PeakCollection '" + peaks->name() + "'");
    peaks->resetIntegrationFlags(_params->integrator_type);
    IIntegrator* integrator = getIntegrator(_params->integrator_type);
    integrator->setParallel(parallel);
    integrator->setParameters(*params);
    integrator->parallelIntegrate(peaks->getPeakList(), shapes, data);
    peaks->setIntegrated(true);
    if (params->use_gradient)
        peaks->setBkgGradient(true);

    _n_peaks = 0;
    _n_valid = 0;
    for (auto peak : peaks->getPeakList()) {
        ++_n_peaks;
        if (peak->enabled())
            ++_n_valid;
    }

    delete integrator;
}

    void Integrator::integrateFoundPeaks(PeakFinder* peak_finder, bool parallel)
{
    ohklLog(Level::Info, "Integrator::integrateFoundPeaks");
    IIntegrator* integrator = getIntegrator(IntegratorType::PixelSum);
    integrator->setParallel(parallel);
    integrator->setParameters(*_params);

    integrator->parallelIntegrate(peak_finder->currentPeaks(), nullptr, peak_finder->currentData());

    _n_peaks = 0;
    _n_valid = 0;
    for (auto peak : peak_finder->currentPeaks()) {
        ++_n_peaks;
        if (peak->enabled())
            ++_n_valid;
    }
    peak_finder->setIntegrated(true);
    if (_params->use_gradient)
        peak_finder->setBkgGradient(true);

    delete integrator;
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
