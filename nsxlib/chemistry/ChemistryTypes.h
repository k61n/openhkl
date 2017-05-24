#ifndef NSXLIB_CHEMISTRYTYPES_H
#define NSXLIB_CHEMISTRYTYPES_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace nsx {

class Material;

using isotopeContents=std::map<std::string,double>;

using sptrMaterial = std::shared_ptr<Material>;

enum class ChemicalPropertyType {String=1, Int=2, Double=3, Complex=4, Bool=5};

using isotopeContents = std::map<std::string,double>;

using compoundList = std::vector<std::pair<isotopeContents,double>>;

} // end namespace nsx

#endif // NSXLIB_CHEMISTRYTYPES_H
