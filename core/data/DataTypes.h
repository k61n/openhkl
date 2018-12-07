#pragma once

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "Variant.h"

namespace nsx {

class BasicFrameIterator;
class DataSet;
class IDataReader;
class IFrameIterator;
class IFrameInterval;
class ThreadedFrameIterator;

using sptrDataSet = std::shared_ptr<DataSet>;

using DataList = std::vector<sptrDataSet>;

using MetaDataMap = std::map<const char*,Variant<int,double,std::string>>;
using MetaDataKeySet = std::set<std::string>;

} // end namespace nsx
