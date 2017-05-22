#ifndef NSXLIB_UTILSTYPES_H
#define NSXLIB_UTILSTYPES_H

#include <memory>

namespace nsx {

class ProgressHandler;

using sptrProgressHandler = std::shared_ptr<ProgressHandler>;

} // end namespace nsx

#endif // NSXLIB_UTILSTYPES_H
