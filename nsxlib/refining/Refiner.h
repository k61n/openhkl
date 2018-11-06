/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon,  Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de


 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include <vector>

#include "CrystalTypes.h"
#include "InstrumentTypes.h"
#include "RefinementBatch.h"

namespace nsx {

//! \class Refiner
//! \brief Class used to refine lattice and instrument parameters.
class Refiner {
public:
    //! Construct an instance to refine the given cell based on the given peak list, using
    //! the given number of frame batches. The peaks must belong to the same dataset.
    Refiner(InstrumentStateList& states, sptrUnitCell cell, const PeakList& peaks, int nbatches);
    //! Set the lattice B matrix to be refined.
    void refineUB();
    //! Set detector offsets in the given list of instrument states to be refined.
    void refineDetectorOffset();
    //! Set the sample position in the given list of instrument states to be refined.
    void refineSamplePosition();
    //! Set the sample orientation in the given list of instrument states to be refined.
    void refineSampleOrientation();
    //! Set the source ki in the given list of instrument states to be refined.
    void refineKi();
    //! Perform the refinement with the maximum number of iterations as given.
    bool refine(unsigned int max_iter = 100);
    //! Update the centers of predicted peaks, after refinement.
    int updatePredictions(PeakList& peaks) const;
    //! Return the individual peak/frame batches used during refinement.
    const std::vector<RefinementBatch>& batches() const;

private:

    sptrUnitCell _cell;

    std::vector<RefinementBatch> _batches;
};

} // end namespace nsx
