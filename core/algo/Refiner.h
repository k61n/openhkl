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

#ifndef CORE_ALGO_REFINER_H
#define CORE_ALGO_REFINER_H

#include "core/algo/RefinementBatch.h"

namespace nsx {

//! Used to refine lattice and instrument parameters.

class Refiner {
 public:
    //! Construct an instance to refine the given cell based on the given peak
    //! list, using the given number of frame batches. The peaks must belong to
    //! the same dataset.
    Refiner(InstrumentStateList& states, UnitCell* cell, std::vector<nsx::Peak3D*> peaks, int nbatches);
    //! Sets the lattice B matrix to be refined.
    void refineUB();
    //! Sets detector offsets in the given list of instrument states to be refined.
    void refineDetectorOffset();
    //! Sets the sample position in the given list of instrument states to be
    //! refined.
    void refineSamplePosition();
    //! Sets the sample orientation in the given list of instrument states to be
    //! refined.
    void refineSampleOrientation();
    //! Sets the source ki in the given list of instrument states to be refined.
    void refineKi();
    //! Perform the refinement with the maximum number of iterations as given.
    bool refine(unsigned int max_iter = 100);
    //! Update the centers of predicted peaks, after refinement.
    int updatePredictions(PeakList& peaks) const;
    //! Returns the individual peak/frame batches used during refinement.
    const std::vector<RefinementBatch>& batches() const;

 private:
 
    UnitCell* _cell;
    std::vector<RefinementBatch> _batches;
};

} // namespace nsx

#endif // CORE_ALGO_REFINER_H
