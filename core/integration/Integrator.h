//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/Integrator.h
//! @brief     Handles integration for Experiment object
//! //! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_EXPERIMENT_INTEGRATOR_H
#define NSX_CORE_EXPERIMENT_INTEGRATOR_H

#include "core/shape/IPeakIntegrator.h"
#include "core/shape/PeakFilter.h"
#include <map>
#include <string>

namespace nsx {

using IntegratorMap = std::map<IntegratorType, std::unique_ptr<nsx::IPeakIntegrator>>;

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

class Integrator {

 public:
    Integrator(std::shared_ptr<DataHandler> data_handler = nullptr);

 public:
    //! Return a pointer to the integrator map
    IntegratorMap* getIntegratorMap();
    //! Return a pointer to the data handler
    DataHandler* getDataHandler();
    //! Get an integrator from the map
    nsx::IPeakIntegrator* getIntegrator(const IntegratorType integrator_type) const;
    //! Set the found peak integrator
    void integratePeaks(
        IntegratorType integrator_type, sptrDataSet data, PeakCollection* peaks);
    //! Integrate a peak collection
    void integratePeaks(
        sptrDataSet data, PeakCollection* peaks, IntegrationParameters* params,
        ShapeCollection* shapes);
    //! Integrate peaks found by _peak_finder
    void integrateFoundPeaks(PeakFinder* peak_finder);
    //! Integrate the shape collection
    void integrateShapeCollection(
        std::vector<Peak3D*>& peaks, sptrDataSet data, ShapeCollection* shape_collection,
        const AABB& aabb, const ShapeCollectionParameters& params);
    //! Set the parameters
    void setParameters(std::shared_ptr<IntegrationParameters> params);
    //! Get the parameters
    IntegrationParameters* parameters();
    //! Set a progress handler
    void setHandler(sptrProgressHandler handler);

 private:
    sptrProgressHandler _handler;
    IntegratorMap _integrator_map;
    std::shared_ptr<DataHandler> _data_handler;
    std::shared_ptr<IntegrationParameters> _params;
};

} // namespace nsx

#endif // NSX_CORE_EXPERIMENT_INTEGRATOR_H
