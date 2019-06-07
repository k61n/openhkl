//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/utils/UtilsTypes.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_UTILS_UTILSTYPES_H
#define CORE_UTILS_UTILSTYPES_H

#include <functional>
#include <memory>
#include <set>
#include <tuple>

namespace nsx {

class ProgressHandler;

using sptrProgressHandler = std::shared_ptr<ProgressHandler>;

using ConstraintTuple = std::tuple<unsigned int, unsigned int, double>;
using ConstraintSet = std::set<ConstraintTuple>;

using FinishedCallback = std::function<void(bool)>;
using TaskCallback = std::function<bool(void)>;

} // end namespace nsx

#endif // CORE_UTILS_UTILSTYPES_H
