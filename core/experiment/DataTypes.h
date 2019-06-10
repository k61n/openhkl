//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/DataTypes.h
//! @brief     Declares classes, defines data types
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_EXPERIMENT_DATATYPES_H
#define CORE_EXPERIMENT_DATATYPES_H

#include <functional>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "core/utils/Variant.h"

namespace nsx {

class DataSet;
class IDataReader;
class IFrameIterator;
class PeakFinder;

using sptrDataSet = std::shared_ptr<DataSet>;
using sptrPeakFinder = std::shared_ptr<PeakFinder>;
using DataList = std::vector<sptrDataSet>;
using FrameIteratorCallback = std::function<IFrameIterator*(DataSet&, int)>;
using MetaDataMap = std::map<const char*, Variant<int, double, std::string>>;
using MetaDataKeySet = std::set<std::string>;

} // namespace nsx

#endif // CORE_EXPERIMENT_DATATYPES_H
