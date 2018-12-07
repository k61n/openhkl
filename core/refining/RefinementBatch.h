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

#include <functional>
#include <map>
#include <vector>

#include <Eigen/SparseCore>

#include "PeakList.h"
#include "FitParameters.h"
#include "InstrumentTypes.h"
#include "ReciprocalVector.h"
#include "UnitCell.h"

namespace nsx {

//! \class RefinementBatch
//! \brief Helper class to represent a batch of consecutive detector images.
class RefinementBatch {

public:
    //! Default constructor. Should not be used but needed for swig
    RefinementBatch() = default;
    //! Construct batch with initial unit cell and list of peaks.
    RefinementBatch(InstrumentStateList& states, const UnitCell& uc, const PeakList& peaksmax);
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
    //! Compute the residual vector for the current set of parameters.
    int residuals(Eigen::VectorXd& fvec);
    //! Return the list of peaks used for refinement.
    const PeakList& peaks() const;
    //! Return the refined unit cell.
    sptrUnitCell cell() const;
    //! Return the matrix of parameter constraints.
    Eigen::MatrixXd constraintKernel() const;
    //! Determine if a given frame number is part of this batch.
    bool contains(double f) const;
    //! Return the cost function
    const std::vector<double>& costFunction() const;

private:

    double _fmin;

    double _fmax;

    sptrUnitCell _cell;

    PeakList _peaks;

    FitParameters _params;

    //! Initial U matrix of cell
    Eigen::Matrix3d _u0; 

    //! U offsets
    Eigen::Vector3d _uOffsets;

    //! Cell parameters, internal format. Used internally by UBMinimizer.
    Eigen::VectorXd _cellParameters;

    std::vector<Eigen::RowVector3d> _hkls;

    std::vector<std::vector<int>> _constraints;

    std::vector<Eigen::Matrix3d> _wts;

    std::vector<std::reference_wrapper<InstrumentState>> _states;

    std::vector<double> _cost_function;
};

} // end namespace nsx
