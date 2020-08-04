//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/IntegrationHandler.h
//! @brief     Handles integration for Experiment object
//! //! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_INTEGRATIONHANDLER_H
#define NSX_CORE_EXPERIMENT_INTEGRATIONHANDLER_H

#include "core/shape/PeakFilter.h"
#include <map>
#include <string>

namespace nsx {

using IntegratorMap = std::map<std::string, std::unique_ptr<nsx::IPeakIntegrator>>;

class PeakCollection;
class PeakFinder;
class GaussianIntegrator;
class ISigmaIntegrator;
class PixelSumIntegrator;
class Profile1DIntegrator;
class Profile3DIntegrator;
class ShapeIntegrator;
class DataHandler;
struct PredictionParameters;

class IntegrationHandler {

 public:
    IntegrationHandler() = default;
    ~IntegrationHandler();
    IntegrationHandler(const IntegrationHandler& other);
    IntegrationHandler& operator=(const IntegrationHandler& other) = default;
    IntegrationHandler(std::shared_ptr<DataHandler> data_handler);

 public:
    //! Return a pointer to the integrator map
    IntegratorMap* getIntegratorMap();
    //! Return a pointer to the data handler
    DataHandler* getDataHandler();
    //! Get an integrator from the map
    nsx::IPeakIntegrator* getIntegrator(const std::string& name) const;
    //! Set the found peak integrator
    void integratePeaks(std::string integrator_name, PeakCollection* peak_collection);
    //! Set the found peak integrator
    void integratePredictedPeaks(
        std::string integrator_name, PeakCollection* peak_collection, ShapeLibrary* shape_library,
        PredictionParameters& params);
    //! Integrate peaks found by _peak_finder
    void integrateFoundPeaks(std::string integrator, PeakFinder* peak_finder);
    //! Integrate the shape library
    ShapeLibrary& integrateShapeLibrary(
        std::vector<Peak3D*>& peaks, ShapeLibrary* shape_library, const AABB& aabb,
        const ShapeLibParameters& params);

 private:
    IntegratorMap _integrator_map;
    std::shared_ptr<DataHandler> _data_handler;
    //! Add integrators to the map
    void populateMap();
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_INTEGRATIONHANDLER_H
