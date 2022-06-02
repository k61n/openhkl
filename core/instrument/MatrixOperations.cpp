//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/MatrixOperations.cpp
//! @brief     Defines functions that operate on matrices
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/MatrixOperations.h"
#include <Eigen/Core>

namespace nsx {

void removeColumn(Eigen::MatrixXd& matrix, unsigned int colToRemove)
{
    unsigned int numRows = matrix.rows();
    unsigned int numCols = matrix.cols() - 1;

    if (colToRemove < numCols)
        matrix.block(0, colToRemove, numRows, numCols - colToRemove) =
            matrix.block(0, colToRemove + 1, numRows, numCols - colToRemove);

    matrix.conservativeResize(numRows, numCols);
}

void removeRow(Eigen::MatrixXd& matrix, unsigned int rowToRemove)
{
    unsigned int numRows = matrix.rows() - 1;
    unsigned int numCols = matrix.cols();

    if (rowToRemove < numRows)
        matrix.block(rowToRemove, 0, numRows - rowToRemove, numCols) =
            matrix.block(rowToRemove + 1, 0, numRows - rowToRemove, numCols);

    matrix.conservativeResize(numRows, numCols);
}

Eigen::Matrix3d interpolateRotation(
    const Eigen::Matrix3d& U0, const Eigen::Matrix3d& U1, const double t)
{
    Eigen::Quaterniond q0(U0), q1(U1);
    return q0.slerp(t, q1).toRotationMatrix();
}

} // namespace nsx
