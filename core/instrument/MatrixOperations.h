//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/MatrixOperations.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INSTRUMENT_MATRIXOPERATIONS_H
#define CORE_INSTRUMENT_MATRIXOPERATIONS_H

#include <Eigen/Dense>

namespace nsx {

void removeColumn(Eigen::MatrixXd& matrix, unsigned int colToRemove);

void removeRow(Eigen::MatrixXd& matrix, unsigned int rowToRemove);

Eigen::Matrix3d
interpolateRotation(const Eigen::Matrix3d& U0, const Eigen::Matrix3d& U1, const double t);

} // end namespace nsx

#endif // CORE_INSTRUMENT_MATRIXOPERATIONS_H
