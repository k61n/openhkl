//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/Predictor.h
//! @brief     Defines classes PeakInterpolation, Predictor
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_SHAPE_PREDICTOR_H
#define OHKL_CORE_SHAPE_PREDICTOR_H

#include "core/data/DataTypes.h"
#include "core/integration/IIntegrator.h"
#include "tables/crystal/MillerIndex.h"

#include <vector>

namespace ohkl {

class Diffractometer;
class InstrumentStateSet;

/*! \addtogroup python_api
 *  @{*/

//! Parameters for peak prediction
struct PredictionParameters : public IntegrationParameters {
    double d_min = 1.5; //!< Minimum detector range (filter)
    double d_max = 50.0; //!< Maximum detector range (filter)

    void log(const Level& level) const;
};

//! Parameters for strategy tool
struct StrategyParameters : public PredictionParameters {
    double delta_chi = 0.0;
    double delta_omega = 0.5;
    double delta_phi = 0.0;
    std::size_t nframes = 200;
    bool friedel = true;

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
    ~Predictor();

    //! Predict peaks give a unit cell
    void predictPeaks(const sptrDataSet data, const sptrUnitCell unit_cell);

    //! Build a list of peaks from hkls as computed from unit cell
    std::vector<Peak3D*> buildPeaksFromMillerIndices(
        sptrDataSet data, const std::vector<MillerIndex>& hkls, const sptrUnitCell unit_cell,
        sptrProgressHandler handler = nullptr);

    //! Predict peaks in strategy mode
    void strategyPredict(sptrDataSet data, const sptrUnitCell unit_cell);

    //! Get a pointer to the prediction parameters
    PredictionParameters* parameters();
    //! Get a pointer to the strategy paramters
    StrategyParameters* strategyParamters();
    //! Get the vector of predicted peaks
    const std::vector<Peak3D*>& peaks() const;
    //! Get the number of predicted peaks
    unsigned int numberOfPredictedPeaks();
    //! Set handler for GUI
    void setHandler(sptrProgressHandler handler);

 private:
    //! Generate instrument states for strategy tool
    InstrumentStateSet generateStates(const sptrDataSet data);

    std::unique_ptr<PredictionParameters> _params;
    std::unique_ptr<StrategyParameters> _strategy_params;
    std::unique_ptr<Diffractometer> _strategy_diffractometer;
    std::unique_ptr<InstrumentStateSet> _strategy_states;
    std::vector<Peak3D*> _predicted_peaks;
    sptrProgressHandler _handler;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_SHAPE_PREDICTOR_H
