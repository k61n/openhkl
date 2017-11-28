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
class MetaData;
class PeakFinder;
class ThreadedFrameIterator;

using sptrDataSet = std::shared_ptr<DataSet>;
using sptrPeakFinder = std::shared_ptr<PeakFinder>;

using uptrMetaData       = std::unique_ptr<MetaData>;
using uptrIDataReader    = std::unique_ptr<IDataReader>;
using uptrIFrameIterator = std::unique_ptr<IFrameIterator>;

using wptrDataSet  = std::weak_ptr<DataSet>;

using DataList = std::vector<sptrDataSet>;

using FrameIteratorCallback = std::function<IFrameIterator*(DataSet&, int)>;

using MetaDataMap = std::map<const char*,Variant<int,double,std::string>>;
using MetaDataKeySet = std::set<std::string>;

} // end namespace nsx
