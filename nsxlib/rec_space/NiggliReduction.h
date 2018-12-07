/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr

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

#include <Eigen/Dense>

namespace nsx {


//! \class NiggliReduction
//! \brief Class to handle reduction of a general unit cell to a Niggli cell.
//! Implementation of primitive unit-cell reduction using the method described in :
//! "Numerically stable algorithms for the computation of reduced Unit cells"
//! by R.W. Grosse-Kunstleve et al. , Acta Cryst., A60, 1-6, (2004).
//! This is the similar with Krivy & Gruber's algorithm but with the additional
//! tolerance (epsilon).
class NiggliReduction {
public:
    //! Niggli reduction of the metric tensor g, with relative error epsilon
    //! The relative error is multiplied internally by the average unit dimension of g
    NiggliReduction(const Eigen::Matrix3d& g, double epsilon);
    //! Reduce the unit-cell and output the reduced metric tensor and the transformation matrix
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
    //! Internal variables storing 1 0 or -1 for each angle (acute, right, or obtuse)
    int _l,_m,_n;
};

} // end namespace nsx
