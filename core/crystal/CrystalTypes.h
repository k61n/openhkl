//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/crystal/CrystalTypes.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_CRYSTAL_CRYSTALTYPES_H
#define CORE_CRYSTAL_CRYSTALTYPES_H

#include <memory>
#include <set>
#include <utility>
#include <vector>

#include <Eigen/Dense>

namespace nsx {

class MillerIndex;
class MergedPeak;
class Peak3D;
class PeakIntegrator;
class PeakValidator;
class Profile;
class SpaceGroup;
class SpaceGroupSymbols;
class SymOp;
class UnitCell;

using MillerIndexList = std::vector<MillerIndex>;

using affineTransformation = Eigen::Transform<double, 3, Eigen::Affine>;

using UnitCellSolution = std::pair<UnitCell, double>;

using sptrPeak3D = std::shared_ptr<Peak3D>;
using sptrUnitCell = std::shared_ptr<UnitCell>;

using RankedSolution = std::pair<sptrUnitCell, double>;

using PeakList = std::vector<sptrPeak3D>;

using MergedPeakSet = std::set<MergedPeak>;

using SymOpList = std::vector<SymOp>;
using UnitCellList = std::vector<sptrUnitCell>;

using SpaceGroupSymmetry = std::pair<std::string, std::string>;

using AutoIndexingSoluce = std::pair<UnitCell, double>;

} // end namespace nsx

#endif // CORE_CRYSTAL_CRYSTALTYPES_H
