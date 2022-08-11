//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/Predictor.h
//! @brief     Defines classes PeakInterpolation, Predictor
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_SHAPE_PREDICTOR_H
#define OHKL_CORE_SHAPE_PREDICTOR_H

#include "base/utils/ProgressHandler.h"
#include "core/integration/IIntegrator.h"
#include "core/shape/ShapeModel.h"

#include <vector>

class Peak3D;

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

//! Parameters for peak prediction
struct PredictionParameters : public IntegrationParameters {
    double d_min = 1.5; //!< Minimum detector range (filter)
    double d_max = 50.0; //!< Maximum detector range (filter)

    void log(const Level& level) const;
};

/*! \brief Predict peaks positions in real space given a unit cell
 *
 *  Given a unit cell, convert all combinations of Miller indices in some range
 *  to q-vectors, and transform their positions to real space detector spots.
 */

class Predictor {
 public:
    Predictor();

    //! Predict peaks give a unit cell
    void predictPeaks(const sptrDataSet data, const sptrUnitCell unit_cell);

    //! Build a list of peaks from hkls as computed from unit cell
    std::vector<Peak3D*> buildPeaksFromMillerIndices(
        sptrDataSet data, const std::vector<MillerIndex>& hkls, const sptrUnitCell unit_cell,
        sptrProgressHandler handler = nullptr);

    //! Get a pointer to the prediction parameters
    PredictionParameters* parameters();
    //! Get the vector of predicted peaks
    const std::vector<Peak3D*>& peaks() const;
    //! Get the number of predicted peaks
    unsigned int numberOfPredictedPeaks();
    //! Set handler for GUI
    void setHandler(sptrProgressHandler handler);

 private:
    std::unique_ptr<PredictionParameters> _params;
    std::vector<Peak3D*> _predicted_peaks;
    sptrProgressHandler _handler;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_SHAPE_PREDICTOR_H
