#pragma once

#include <memory>
#include <vector>

namespace nsx {

class DataSet;

using sptrDataSet = std::shared_ptr<DataSet>;

using DataList = std::vector<sptrDataSet>;

} // end namespace nsx
