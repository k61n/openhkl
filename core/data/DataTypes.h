#pragma once

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "Variant.h"

namespace nsx {

class DataSet;
class IDataReader;
class IFrameIterator;

using sptrDataSet = std::shared_ptr<DataSet>;

using DataList = std::vector<sptrDataSet>;

} // end namespace nsx
