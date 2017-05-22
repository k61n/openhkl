#ifndef NSXLIB_CHEMISTRYTYPES_H
#define NSXLIB_CHEMISTRYTYPES_H

#include <map>
#include <memory>

namespace nsx {

class Material;

using isotopeContents=std::map<std::string,double>;

using sptrMaterial = std::shared_ptr<Material>;

} // end namespace nsx

#endif // NSXLIB_CHEMISTRYTYPES_H
