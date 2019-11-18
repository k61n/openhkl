//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/RefinementBatch.h
//! @brief     Defines class RefinementBatch
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_ALGO_REFINEMENTBATCH_H
#define CORE_ALGO_REFINEMENTBATCH_H

#include "base/fit/FitParameters.h"
#include "core/instrument/InstrumentTypes.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"

namespace nsx {

//! Represents a batch of consecutive detector images.

class RefinementBatch {
 public:
    //! Default constructor. Should not be used but needed for swig
    RefinementBatch() = default;
    //! Construct batch with initial unit cell and list of peaks.
    RefinementBatch(InstrumentStateList& states, UnitCell* uc, std::vector<nsx::Peak3D*> peaksmax);
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
    //! Compute the residual vector for the current set of parameters.
    int residuals(Eigen::VectorXd& fvec);
    //! Returns the list of peaks used for refinement.
    std::vector<nsx::Peak3D*> peaks() const;
    //! Returns the refined unit cell.
    UnitCell* cell() const;
    //! Returns the matrix of parameter constraints.
    Eigen::MatrixXd constraintKernel() const;
    //! Determine if a given frame number is part of this batch.
    bool contains(double f) const;
    //! Returns the cost function
    const std::vector<double>& costFunction() const;

 private:
    double _fmin;

    double _fmax;

    nsx::UnitCell* _cell;

    std::vector<nsx::Peak3D*> _peaks;

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

} // namespace nsx

#endif // CORE_ALGO_REFINEMENTBATCH_H
