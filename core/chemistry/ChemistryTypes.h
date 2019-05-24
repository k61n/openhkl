#ifndef CORE_CHEMISTRY_CHEMISTRYTYPES_H
#define CORE_CHEMISTRY_CHEMISTRYTYPES_H

#include <complex>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Variant.h"

namespace nsx {

using isotopeContents = std::map<std::string, double>;

enum class ChemicalPropertyType { String = 1, Int = 2, Double = 3, Complex = 4, Bool = 5 };

using isotopeContents = std::map<std::string, double>;

using compoundList = std::vector<std::pair<isotopeContents, double>>;

using isotopeProperties =
    std::map<std::string, Variant<bool, int, double, std::complex<double>, std::string>>;

} // end namespace nsx

#endif // CORE_CHEMISTRY_CHEMISTRYTYPES_H
