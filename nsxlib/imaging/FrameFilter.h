#include <functional>

#include "IData.h"

namespace nsx {

using FrameFilter = std::function<RowMatrixd(RowMatrixd)>;

}
