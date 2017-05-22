#ifndef NSXLIB_DATATYPES_H
#define NSXLIB_DATATYPES_H

#include <functional>
#include <memory>

#include <vector>

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

using uptrMetaData = std::unique_ptr<MetaData>;
using uptrIDataReader = std::unique_ptr<IDataReader>;

using DataList = std::vector<sptrDataSet>;

using FrameIteratorCallback = std::function<IFrameIterator*(DataSet&, int)>;

} // end namespace nsx

#endif // NSXLIB_DATATYPES_H
