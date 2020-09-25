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

namespace nsx {

class UnitCellHandler;

//! Used to refine lattice and instrument parameters.
class Refiner {
 public:
    //! Construct an instance to refine the given cell based on the given peak list,
    //! using the given number of frame batches. The peaks must belong to the same dataset.
    Refiner(
        InstrumentStateList& states, UnitCell* cell, std::vector<nsx::Peak3D*> peaks,
        int nbatches, UnitCellHandler* cell_handler);

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
    bool refine(unsigned int max_iter = 100);

    //! Update the centers of predicted peaks, after refinement.
    int updatePredictions(std::vector<Peak3D*> peaks) const;

    //! Returns the individual peak/frame batches used during refinement.
    const std::vector<RefinementBatch>& batches() const;

    //! Write the initial and final cells to the log
    void logChange();

 private:
    UnitCellHandler* _cell_handler;
    UnitCell _unrefined_cell;
    UnitCell* _cell;
    std::vector<RefinementBatch> _batches;
};

} // namespace nsx

#endif // NSX_CORE_ALGO_REFINER_H
