//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/mathematics/MathematicsTypes.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_MATHEMATICS_MATHEMATICSTYPES_H
#define CORE_MATHEMATICS_MATHEMATICSTYPES_H

#include <Eigen/Dense>

namespace nsx {

using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RealMatrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

} // end namespace nsx

#endif // CORE_MATHEMATICS_MATHEMATICSTYPES_H
