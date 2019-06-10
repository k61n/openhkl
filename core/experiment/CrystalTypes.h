//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/CrystalTypes.h
//! @brief     Declares classes, defines data types
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_EXPERIMENT_CRYSTALTYPES_H
#define CORE_EXPERIMENT_CRYSTALTYPES_H

#include <memory>
#include <vector>

#include <Eigen/Dense>

namespace nsx {

class Peak3D;

using affineTransformation = Eigen::Transform<double, 3, Eigen::Affine>;
using sptrPeak3D = std::shared_ptr<Peak3D>;
using PeakList = std::vector<sptrPeak3D>;

} // namespace nsx

#endif // CORE_EXPERIMENT_CRYSTALTYPES_H
