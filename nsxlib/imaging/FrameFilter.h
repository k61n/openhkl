#ifndef NSXLIB_FRAMEFILTER_H
#define NSXLIB_FRAMEFILTER_H

#include <functional>

namespace nsx {

using FrameFilter = std::function<RowMatrixd(RowMatrixd)>;

} // end namespace nsx

#endif // NSXLIB_FRAMEFILTER_H
