//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/Refiner.h
//! @brief     Defines class Refiner
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_ALGO_REFINER_H
#define NSX_CORE_ALGO_REFINER_H

#include "core/algo/RefinementBatch.h"
#include "core/experiment/UnitCellHandler.h"
#include "core/shape/IPeakIntegrator.h"
#include "tables/crystal/UnitCell.h"
#include <vector>

namespace nsx {

class ProgressHandler;
using sptrProgressHandler = std::shared_ptr<ProgressHandler>;
enum class Level;

//! Parameters for refinement
struct RefinerParameters {
    bool refine_ub = true;
    bool refine_ki = true;
    bool refine_sample_position = true;
    bool refine_sample_orientation = true;
    bool refine_detector_offset = true;
    int nbatches = 10;
    ResidualType residual_type = ResidualType::QSpace;
    unsigned int max_iter = 1000;

    void log(const Level& level) const;
};

//! Used to refine lattice and instrument parameters.
class Refiner {
 public:
    Refiner(UnitCellHandler* cell_handler);

    void setHandler(const sptrProgressHandler& handler);

    //! Generate batches of peaks per frame range with the given peak list
    void makeBatches(
        InstrumentStateList& states, const std::vector<nsx::Peak3D*>& peaks,
        sptrUnitCell cell = nullptr);

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
    //! previous funcitons set the number of free parameters and at least one must be run
    //! *before* refine
    bool refine();

    //! Update the centers of predicted peaks, after refinement.
    int updatePredictions(std::vector<Peak3D*>& peaks) const;

    //! Returns the individual peak/frame batches used during refinement.
    const std::vector<RefinementBatch>& batches() const;

    //! Return the unrefined cell
    UnitCell* unrefinedCell();

    //! Return the refined states
    InstrumentStateList* refinedStates();

    //! Return the unrefined states
    InstrumentStateList* unrefinedStates();

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

 private :
    //! Determine which unit cell to use in a batch
    sptrUnitCell _getUnitCell(const std::vector<Peak3D*> peaks_subset, sptrUnitCell cell);

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

} // namespace nsx

#endif // NSX_CORE_ALGO_REFINER_H
