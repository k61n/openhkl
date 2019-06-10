//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/loader/DataReaderFactory.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_LOADER_DATAREADERFACTORY_H
#define CORE_LOADER_DATAREADERFACTORY_H

namespace nsx {

/** \brief DataReaderFactory. All IData formats must register their "create"
 * method with the factory in order to choose the correct DataReader at runtime.
 * Reader selection is based on the extension of the datafile.
 *
 */
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

#endif // CORE_LOADER_DATAREADERFACTORY_H
