//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/NiggliReduction.h
//! @brief     Defines class NiggliReduction
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_TABLES_CRYSTAL_NIGGLIREDUCTION_H
#define NSX_TABLES_CRYSTAL_NIGGLIREDUCTION_H

#include <Eigen/Dense>

namespace ohkl {

//! Class to handle reduction of a general unit cell to a Niggli cell.

//! Implementation of primitive unit-cell reduction using the method described
//! in : "Numerically stable algorithms for the computation of reduced Unit
//! cells" by R.W. Grosse-Kunstleve et al. , Acta Cryst., A60, 1-6, (2004). This
//! is the similar with Krivy & Gruber's algorithm but with the additional tolerance (epsilon).

class NiggliReduction {
 public:
    //! Niggli reduction of the metric tensor g, with relative error epsilon
    //! The relative error is multiplied internally by the average unit dimension
    //! of g
    NiggliReduction(const Eigen::Matrix3d& g, double epsilon);
    //! Reduce the unit-cell and output the reduced metric tensor and the
    //! transformation matrix
    void reduce(Eigen::Matrix3d& newg, Eigen::Matrix3d& P);
    //! Change the maximum allowed number of iterations
    static void setIterMax(unsigned int max);

 private:
    //! Number of iteration max allowed in the reduction procedure.
    static unsigned int _itermax;
    //! Relative error
    double _epsilon;
    //! TransformG with current C-matrix transformation
    void transformG();
    //! Update the Niggli parameters
    void updateParameters();
    //! Current metric tensor
    Eigen::Matrix3d _g;
    //! Current transformation matrix
    Eigen::Matrix3d _P;
    //! Store transformation matrix
    Eigen::Matrix3d _CMat;
    //! Niggli parameters
    double _A, _B, _C, _zeta, _eta, _xi;
    //! Internal variables storing 1 0 or -1 for each angle (acute, right, or
    //! obtuse)
    int _l, _m, _n;
};

} // namespace ohkl

#endif // NSX_TABLES_CRYSTAL_NIGGLIREDUCTION_H
