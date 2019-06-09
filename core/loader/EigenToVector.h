//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/EigenToVector.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOADER_EIGENTOVECTOR_H
#define CORE_LOADER_EIGENTOVECTOR_H

#include <vector>

#include <Eigen/Dense>

namespace nsx {

std::vector<double> eigenToVector(const Eigen::VectorXd& ev);

} // namespace nsx

#endif // CORE_LOADER_EIGENTOVECTOR_H
