//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/Integrator.h
//! @brief     Handles integration for Experiment object
//! //! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OPENHKL_CORE_EXPERIMENT_INTEGRATOR_H
#define OPENHKL_CORE_EXPERIMENT_INTEGRATOR_H

#include "core/data/DataTypes.h"
#include "core/integration/IIntegrator.h"

#include <map>
#include <string>

namespace ohkl {

using IntegratorMap = std::map<IntegratorType, std::unique_ptr<ohkl::IIntegrator>>;

class PeakCollection;
class PeakFinder;
class DataHandler;
struct ShapeModelParameters;

/*! \addtogroup python_api
 *  @{*/

/*! \brief Integrate predicted peaks
 *
 *  Implements naive pixel sum integration and various profile fitting
 *  integration methods for computing intensities, sigmas and strengths of
 *  reflections.
 *
 *  Construct a shape collection of integrated strong peaks, and compute the
 *  mean covariance of shapes in the vicinity of a predicted peaks to generate
 *  an integration region. This is integrated via profile integration.
 */
class Integrator {
 public:
    Integrator(std::shared_ptr<DataHandler> data_handler = nullptr);

    //! Get an integrator from the map
    ohkl::IIntegrator* getIntegrator(const IntegratorType integrator_type) const;
    //! Return a pointer to the data handler
    DataHandler* getDataHandler();
    //! Set the found peak integrator
    void integratePeaks(IntegratorType integrator_type, sptrDataSet data, PeakCollection* peaks);
    //! Integrate a peak collection
    void integratePeaks(
        sptrDataSet data, PeakCollection* peaks, IntegrationParameters* params, ShapeModel* shapes);
    //! Integrate peaks found by _peak_finder
    void integrateFoundPeaks(PeakFinder* peak_finder);
    //! Integrate the shape collection
    void integrateShapeModel(
        std::vector<Peak3D*> peaks, sptrDataSet data, ShapeModel* shape_model, const AABB& aabb,
        const ShapeModelParameters& params);
    //! Set the parameters
    void setParameters(std::shared_ptr<IntegrationParameters> params);
    //! Get the parameters
    IntegrationParameters* parameters();
    //! Set a progress handler
    void setHandler(sptrProgressHandler handler);

    //! Get the number of valid peaks;
    unsigned int numberOfPeaks();
    //! Get the total number of peaks
    unsigned int numberOfValidPeaks();

 private:
    sptrProgressHandler _handler;
    IntegratorMap _integrator_map;
    std::shared_ptr<DataHandler> _data_handler;
    std::shared_ptr<IntegrationParameters> _params;

    unsigned int _n_peaks;
    unsigned int _n_valid;
};

/*! @}*/
} // namespace ohkl

#endif // OPENHKL_CORE_EXPERIMENT_INTEGRATOR_H
