//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/parser/EigenToVector.h
//! @brief     Declares function eigenToVector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_BASE_PARSER_EIGENTOVECTOR_H
#define NSX_BASE_PARSER_EIGENTOVECTOR_H

#include <vector>

#include <Eigen/Dense>

namespace nsx {

std::vector<double> eigenToVector(const Eigen::VectorXd& ev);

} // namespace nsx

#endif // NSX_BASE_PARSER_EIGENTOVECTOR_H
