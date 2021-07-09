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
#include "core/shape/IPeakIntegrator.h"

namespace nsx {

class UnitCellHandler;
enum class Level;

//! Parameters for refinement
struct RefinerParameters {
    bool refine_ub = true;
    bool refine_ki = true;
    bool refine_sample_position = true;
    bool refine_sample_orientation = true;
    bool refine_detector_offset = true;
    int nbatches = 10;
    unsigned int max_iter = 1000;

    void log(const Level& level) const;
};

//! Used to refine lattice and instrument parameters.
class Refiner {
 public:
    //! Construct an instance to refine the given cell based on the given peak list,
    //! using the given number of frame batches. The peaks must belong to the same dataset.
    Refiner(
        InstrumentStateList& states, UnitCell* cell, const std::vector<nsx::Peak3D*>& peaks,
        UnitCellHandler* cell_handler, int nbatches);

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

    //! Write the initial and final cells to the log
    void logChange();

    //! set the refiner parameters
    void setParameters(std::shared_ptr<RefinerParameters> params);

    //! get a pointer to the parameters
    RefinerParameters* parameters();

 private:
    UnitCellHandler* _cell_handler;
    UnitCell _unrefined_cell;
    InstrumentStateList _unrefined_states;
    UnitCell* _cell;
    std::vector<RefinementBatch> _batches;
    int _nframes;
    InstrumentStateList* _states;
    std::shared_ptr<RefinerParameters> _params;
};

} // namespace nsx

#endif // NSX_CORE_ALGO_REFINER_H
