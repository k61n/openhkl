#ifndef NSXLIB_DATATYPES_H
#define NSXLIB_DATATYPES_H

#include <memory>

#include <vector>

namespace nsx {

class DataSet;
class PeakFinder;

using sptrDataSet = std::shared_ptr<DataSet>;
using sptrPeakFinder = std::shared_ptr<PeakFinder>;

using DataList = std::vector<sptrDataSet>;

} // end namespace nsx

#endif // NSXLIB_DATATYPES_H
