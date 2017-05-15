#include <functional>

#include "IData.h"

namespace nsx {
    namespace Imaging {
        using FrameFilter = std::function<nsx::Data::RowMatrixd(nsx::Data::RowMatrixd)>;
    }
}
