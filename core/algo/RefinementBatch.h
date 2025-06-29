//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/algo/RefinementBatch.h
//! @brief     Defines class RefinementBatch
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_ALGO_REFINEMENTBATCH_H
#define OHKL_CORE_ALGO_REFINEMENTBATCH_H

#include "base/fit/FitParameters.h"
#include "core/data/DataTypes.h"
#include "core/instrument/InstrumentState.h"

namespace ohkl {

class Peak3D;

enum class ResidualType { RealSpace, QSpace, Count };

//! Represents a batch of consecutive detector images.

class RefinementBatch {
 public:
    //! Default constructor. Should not be used but needed for swig
    RefinementBatch() = default;
    //! A batch contains peaks from frame _fmin to _fmax + 2
    RefinementBatch(InstrumentStateList& states, sptrUnitCell uc, std::vector<ohkl::Peak3D*> peaks);

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

    //! Perform the refinement with the maximum number of iterations as given.
    bool refine(unsigned int max_iter = 100);

    //! Compute the residual vector for the current set of parameters.
    int residuals(Eigen::VectorXd& fvec) const;

    //! Compute reciprocal space residual
    int qSpaceResiduals(Eigen::VectorXd& fvec) const;

    //! Compute real (detector) space residuals
    int realSpaceResiduals(Eigen::VectorXd& fvec) const;

    //! Returns the list of peaks used for refinement.
    std::vector<ohkl::Peak3D*> peaks() const;

    //! Returns the refined unit cell.
    UnitCell* cell() const;

    //! Return shared pointer to the refined unit cell
    sptrUnitCell sptrCell() const;


    //! Return the lower frame bound
    int first_frame() const;

    //! Return the upper frame bound
    int last_frame() const;

    //! Determine if a given frame number is part of this batch (there is an overlap).
    bool contains(double f) const;

    //! Determine if a given frame number is part of *only* this batch
    bool onlyContains(double f) const;

    //! Generate a name from the minimum/maximum frame
    std::string name() const;

    //! Set the residual type
    void setResidualType(const ResidualType& residual);

    double fmin() { return _fmin; };
    double fmax() { return _fmax; };

 private:
    double _fmin;
    double _fmax;

    ResidualType _residual_type;

    sptrUnitCell _cell;

    std::vector<ohkl::Peak3D*> _peaks;

    FitParameters _params;

    Eigen::Matrix3d _u0; //!< Initial U matrix of cell
    Eigen::Vector3d _uOffsets; //!< U offsets

    Eigen::VectorXd _cellParameters; //!< Used internally by UBMinimizer.

    std::vector<Eigen::RowVector3d> _hkls;
    std::vector<std::vector<int>> _constraints;
    std::vector<Eigen::Matrix3d> _wts;
    std::vector<std::reference_wrapper<InstrumentState>> _states;
};

} // namespace ohkl

#endif // OHKL_CORE_ALGO_REFINEMENTBATCH_H
