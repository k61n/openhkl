#include <functional>

#include "../data/IData.h"

namespace nsx {

using FrameFilter = std::function<RowMatrixd(RowMatrixd)>;

}
