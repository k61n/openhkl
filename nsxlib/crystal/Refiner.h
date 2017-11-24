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

#include "CrystalTypes.h"
#include "FitParameters.h"
#include "InstrumentTypes.h"
#include "ReciprocalVector.h"
#include "UnitCell.h"


namespace nsx {

//! Class for batch refinement of UB and offset
class RefinementBatch {
public:
    RefinementBatch(const UnitCell& uc, const PeakList& peaks, double fmin, double fmax);

    void refineU();
    void refineB();

    void refineDetectorState(InstrumentStateList& states, unsigned int axis);
    void refineSampleState(InstrumentStateList& states, unsigned int axis);
    void refineSourceState(InstrumentStateList& states, unsigned int axis);

    bool refine(unsigned int max_iter = 100);
    int residuals(Eigen::VectorXd& fvec);

private:
    const double _fmin;
    const double _fmax;
    FitParameters _params;
    //! Initial U matrix of cell
    Eigen::Matrix3d _u0; 
    //! U offsets
    Eigen::Vector3d _uOffsets;
    //! Cell parameters, internal format. Used internally by UBMinimizer.
    Eigen::VectorXd _cellParameters;
    UnitCell _cell;
    PeakList _peaks;
    std::vector<Eigen::RowVector3d> _hkl;
};

//! Class for UB and offset refinement
class Refiner {
public:
    Refiner(sptrUnitCell cell, const PeakList& peaks, int nbatches);

    void refineU();
    void refineB();

    void refineDetectorState(InstrumentStateList& states, unsigned int axis);
    void refineSampleState(InstrumentStateList& states, unsigned int axis);
    void refineSourceState(InstrumentStateList& states, unsigned int axis);

    bool refine(unsigned int max_iter = 100);

    //int residuals(Eigen::VectorXd& fvec);

private:
    std::vector<RefinementBatch> _batches;
};

} // end namespace nsx
