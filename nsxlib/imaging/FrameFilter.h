#include <functional>

#include "IData.h"

namespace nsx {

using FrameFilter = std::function<nsx::Data::RowMatrixd(nsx::Data::RowMatrixd)>;

}
