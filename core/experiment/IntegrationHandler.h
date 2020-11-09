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
    void integratePeaks(
        std::string integrator_name, PeakCollection* peak_collection, double d_min, double d_max);
    //! Integrate a peak collection
    void integratePeaks(
        IPeakIntegrator* integrator, PeakCollection* peaks, IntegrationParameters* params,
        ShapeCollection* shapes);
    //! Set the found peak integrator
    void integratePredictedPeaks(
        std::string integrator_name, PeakCollection* peak_collection,
        ShapeCollection* shape_collection, PredictionParameters& params);
    //! Integrate peaks found by _peak_finder
    void integrateFoundPeaks(std::string integrator, PeakFinder* peak_finder);
    //! Integrate the shape collection
    ShapeCollection& integrateShapeCollection(
        std::vector<Peak3D*>& peaks, ShapeCollection* shape_collection, const AABB& aabb,
        const ShapeCollectionParameters& params);

 private:
    IntegratorMap _integrator_map;
    std::shared_ptr<DataHandler> _data_handler;
    //! Add integrators to the map
    void populateMap();
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_INTEGRATIONHANDLER_H
