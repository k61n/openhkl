//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/DataReaderFactory.h
//! @brief     Defines class DataReaderFactory
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_ALGO_DATAREADERFACTORY_H
#define NSX_CORE_ALGO_DATAREADERFACTORY_H

#include "core/data/DataSet.h"

namespace nsx {

//! Factory that gives access to the data readers from core/loader.

//! All IData formats must register their "create" method with the factory in order
//! to choose the correct DataReader at runtime. Reader selection is based on the
//! extension of the datafile.

class DataReaderFactory {
 public:
    //! Callback type of the factory
    using callback = std::function<std::shared_ptr<DataSet>(const std::string&, Diffractometer*)>;

    DataReaderFactory();

    std::shared_ptr<DataSet> create(
        const std::string& extension, const std::string& filename,
        Diffractometer* diffractometer) const;

 private:
    std::map<std::string, callback> _callbacks;
};

} // namespace nsx

#endif // NSX_CORE_ALGO_DATAREADERFACTORY_H
