//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/chemistry/ChemistryTypes.h
//! @brief     Defines class ChemicalPropertyType
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_CHEMISTRY_CHEMISTRYTYPES_H
#define CORE_CHEMISTRY_CHEMISTRYTYPES_H

#include "base/utils/Variant.h"

#include <complex>
#include <map>
#include <vector>

namespace nsx {

enum class ChemicalPropertyType { String = 1, Int = 2, Double = 3, Complex = 4, Bool = 5 };

using isotopeContents = std::map<std::string, double>;

using compoundList = std::vector<std::pair<isotopeContents, double>>;

using isotopeProperties =
    std::map<std::string, Variant<bool, int, double, std::complex<double>, std::string>>;

} // namespace nsx

#endif // CORE_CHEMISTRY_CHEMISTRYTYPES_H
