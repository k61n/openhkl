#ifndef NSXLIB_UTILSTYPES_H
#define NSXLIB_UTILSTYPES_H

#include <memory>
#include <set>
#include <tuple>

namespace nsx {

class ProgressHandler;

using sptrProgressHandler = std::shared_ptr<ProgressHandler>;

using ConstraintTuple = std::tuple<unsigned int,unsigned int,double>;
using ConstraintSet = std::set<ConstraintTuple>;

} // end namespace nsx

#endif // NSXLIB_UTILSTYPES_H
