#ifndef NSXLIB_FRAMEFILTER_H_
#define NSXLIB_FRAMEFILTER_H_

#include <functional>

namespace nsx {

using FrameFilter = std::function<RowMatrixd(RowMatrixd)>;

} // end namespace nsx

#endif // NSXLIB_FRAMEFILTER_H_
