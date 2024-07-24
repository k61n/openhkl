//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/convolve/IntegratorFactory.cpp
//! @brief     Implements class IntegratorFactory
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/integration/IntegratorFactory.h"

#include "core/integration/GaussianIntegrator.h"
#include "core/integration/IIntegrator.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "core/peak/Peak3D.h"


namespace ohkl {

template <typename T> T* create_integrator()
{
    return new T();
}

IntegratorFactory::IntegratorFactory() : _callbacks()
{
    _callbacks[IntegratorType::PixelSum] = &create_integrator<PixelSumIntegrator>;
    _callbacks[IntegratorType::Profile3D] = &create_integrator<Profile3DIntegrator>;
    _callbacks[IntegratorType::Profile1D] = &create_integrator<Profile1DIntegrator>;
    _callbacks[IntegratorType::ISigma] = &create_integrator<ISigmaIntegrator>;
    _callbacks[IntegratorType::Gaussian] = &create_integrator<GaussianIntegrator>;
}

IIntegrator* IntegratorFactory::create(const IntegratorType& integrator_type) const
{
    const auto it = _callbacks.find(integrator_type);

    if (it == _callbacks.end())
        throw std::runtime_error("IntegratorFactory::create: Invalid integrator type");

    return it->second();
}

const std::map<IntegratorType, IntegratorFactory::callback>& IntegratorFactory::callbacks() const
{
    return _callbacks;
}

} // namespace ohkl
