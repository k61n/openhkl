//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/data/DataTypes.h
//! @brief     Declares classes, defines data types
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_DATA_DATATYPES_H
#define OHKL_CORE_DATA_DATATYPES_H

#include <memory>
#include <vector>

namespace ohkl {

class DataSet;
using sptrDataSet = std::shared_ptr<DataSet>;
using DataList = std::vector<sptrDataSet>;

enum class DataFormat { Unknown = -1, OHKL = 0, TIFF, RAW, NEXUS };

} // namespace ohkl

#endif // OHKL_CORE_DATA_DATATYPES_H
