#ifndef NSXLIB_GEOMETRYTYPES_H
#define NSXLIB_GEOMETRYTYPES_H

#include <functional>

namespace nsx {

using FilterCallback = std::function<RealMatrix(const RealMatrix&)>;

} // end namespace nsx

#endif // NSXLIB_GEOMETRYTYPES_H
