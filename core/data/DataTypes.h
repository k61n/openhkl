//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/data/DataTypes.h
//! @brief     Declares classes, defines data types
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_DATA_DATATYPES_H
#define NSX_CORE_DATA_DATATYPES_H

#include <memory>
#include <vector>

namespace nsx {

class DataSet;
using sptrDataSet = std::shared_ptr<DataSet>;
using DataList = std::vector<sptrDataSet>;

// NOTE: `Count` is used to compute the number of members in the enum class
enum class DataFormat { NSX, RAW, NEXUS, Count };

} // namespace nsx

#endif // NSX_CORE_DATA_DATATYPES_H
