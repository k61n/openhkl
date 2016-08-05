#include <functional>

#include "IData.h"

namespace SX {
    namespace Imaging {
        using FrameFilter = std::function<SX::Data::RowMatrixd(SX::Data::RowMatrixd)>;
    }
}
