//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/DataTypes.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_DATA_DATATYPES_H
#define CORE_DATA_DATATYPES_H

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
class MetaData;
class PeakFinder;
class ThreadedFrameIterator;

using sptrDataSet = std::shared_ptr<DataSet>;
using sptrPeakFinder = std::shared_ptr<PeakFinder>;

using uptrIDataReader = std::unique_ptr<IDataReader>;
using uptrIFrameIterator = std::unique_ptr<IFrameIterator>;

using wptrDataSet = std::weak_ptr<DataSet>;

using DataList = std::vector<sptrDataSet>;

using FrameIteratorCallback = std::function<IFrameIterator*(DataSet&, int)>;

using MetaDataMap = std::map<const char*, Variant<int, double, std::string>>;
using MetaDataKeySet = std::set<std::string>;

} // end namespace nsx

#endif // CORE_DATA_DATATYPES_H
