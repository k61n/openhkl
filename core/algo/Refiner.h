//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/algo/Refiner.h
//! @brief     Defines class Refiner
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_ALGO_REFINER_H
#define OHKL_CORE_ALGO_REFINER_H

#include "core/algo/RefinementBatch.h"
#include "core/experiment/UnitCellHandler.h"
#include "core/integration/IIntegrator.h"
#include "tables/crystal/UnitCell.h"
#include <vector>

namespace ohkl {

class ProgressHandler;
using sptrProgressHandler = std::shared_ptr<ProgressHandler>;
enum class Level;

/*! \addtogroup python_api
 *  @{*/

//! Parameters for refinement
struct RefinerParameters {
    //! Refine unit cell
    bool refine_ub = true;
    //! Refine incident wavevector
    bool refine_ki = true;
    //! Refine sample position
    bool refine_sample_position = true;
    //! Refine sample orientation
    bool refine_sample_orientation = true;
    //! Refine detector offset
    bool refine_detector_offset = true;
    //! Number of refinement batches
    int nbatches = 10;
    //! Type of residual used in minimisation
    ResidualType residual_type = ResidualType::QSpace;
    //! Maximum number of iteration for minimisation
    unsigned int max_iter = 1000;
    //! Use unit cells assigned to batches in further refinement
    bool use_batch_cells = true;
    //! Assign unit cell to peaks when making batches
    bool set_unit_cell = true;

    void log(const Level& level) const;
};

/*! \addtogroup python_api
 *  @{*/

/*! \brief Refine unit cell and instrument states
 *
 * Use GSL non-linear least squares minimisation to refine instrument states
 * (sample position and orientation, incident wavevector, and detector position)
 * and unit cell parameters. The residual is computed as the difference between
 * a q-vector computed from the unit cell (used to predict peaks) and a q-vector
 * from transforming an observed reflection into reciprocal space. An
 * alternative residual (useful for refining unit cells and incident
 * wavevectors) is computed using the real space difference between a predicted
 * spot and its associated observed spot.
 */

class Refiner {
 public:
    Refiner(UnitCellHandler* cell_handler);

    //! Generate batches of peaks per frame range with the given peak list
    void makeBatches(
        InstrumentStateList& states, const std::vector<ohkl::Peak3D*>& peaks, sptrUnitCell cell);

    //! Rebuild old batches if refinement failed
    void reconstructBatches(std::vector<Peak3D*> peaks);

    //! Sets the lattice B matrix to be refined.
    void refineUB();

    //! Sets detector offsets in the given list of instrument states to be refined.
    void refineDetectorOffset();

    //! Sets the sample position in the given list of instrument states to be refined.
    void refineSamplePosition();

    //! Sets the sample orientation in the given list of instrument states to be refined.
    void refineSampleOrientation();

    //! Sets the source ki in the given list of instrument states to be refined.
    void refineKi();

    //! Perform the refinement with the maximum number of iterations as given. N.B. the four
    //! previous functions set the number of free parameters and at least one must be run
    //! *before* refine
    bool refine();

    //! Updates the centers of predicted peaks, after refinement.
    //! Returns the number of remaining peaks
    //! (some peaks may be rejected with flag PredictionUpdateFailure).
    int updatePredictions(std::vector<Peak3D*> peaks);

    //! Returns the individual peak/frame batches used during refinement.
    const std::vector<RefinementBatch>& batches() const;

    //! Return the unrefined cell
    const UnitCell* unrefinedCell() const;

    //! Return the refined states
    const InstrumentStateList* refinedStates() const;

    //! Return the unrefined states
    const InstrumentStateList* unrefinedStates() const;

    //! Return number of frames
    int nframes() const;

    //! Check if this is the first refinement
    bool firstRefine() const;

    //! Write the initial and final cells to the log
    void logChange();

    //! get a pointer to the parameters
    RefinerParameters* parameters();

    //! set the parameters
    void setParameters(const RefinerParameters& params);

    //! Assign batch cells to predicted peaks
    void assignPredictedCells(std::vector<Peak3D*> predicted_peaks);

 private:
    //! Determine which unit cell to use in a batch
    sptrUnitCell _getUnitCell(const std::vector<Peak3D*> peaks_subset);

    UnitCellHandler* _cell_handler;
    UnitCell _unrefined_cell;
    InstrumentStateList _unrefined_states;
    sptrUnitCell _cell;
    std::vector<UnitCell*> _batch_cells;
    std::vector<RefinementBatch> _batches;
    int _nframes;
    bool _first_refine = true;
    InstrumentStateList* _states;
    std::unique_ptr<RefinerParameters> _params;
    static constexpr double _eps_norm = 50.0;
    std::vector<sptrUnitCell> _tmp_vec;
    std::vector<Peak3D*> _peaks;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_ALGO_REFINER_H
