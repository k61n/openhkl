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
#include <set>
#include <vector>

#include <Eigen/Dense>

namespace nsx {

class MergedPeak;
class Peak3D;
class SymOp;

using affineTransformation = Eigen::Transform<double, 3, Eigen::Affine>;
using sptrPeak3D = std::shared_ptr<Peak3D>;
using PeakList = std::vector<sptrPeak3D>;
using MergedPeakSet = std::set<MergedPeak>;
using SymOpList = std::vector<SymOp>;
using SpaceGroupSymmetry = std::pair<std::string, std::string>;

} // namespace nsx

#endif // CORE_EXPERIMENT_CRYSTALTYPES_H
