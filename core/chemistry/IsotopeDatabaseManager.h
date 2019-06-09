//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/chemistry/IsotopeDatabaseManager.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_CHEMISTRY_ISOTOPEDATABASEMANAGER_H
#define CORE_CHEMISTRY_ISOTOPEDATABASEMANAGER_H


#include <yaml-cpp/yaml.h>

#include "core/chemistry/ChemistryTypes.h"
#include "core/utils/Singleton.h"

namespace nsx {

//! \brief Class to manage the YAML-based isotope database.
class IsotopeDatabaseManager : public Singleton<IsotopeDatabaseManager, Constructor, Destructor> {

public:
    static std::map<std::string, ChemicalPropertyType> PropertyTypes;

    IsotopeDatabaseManager();

    ~IsotopeDatabaseManager() = default;

    const std::map<std::string, isotopeProperties>& isotopes() const;

    unsigned int nIsotopes() const;

    template <typename T> T property(const std::string& isotope, const std::string& property) const;

    bool hasProperty(const std::string& isotope, const std::string& property) const;

    const std::map<std::string, std::pair<std::string, std::string>>& properties() const;

private:
    std::map<std::string, isotopeProperties> _isotopes;

    std::map<std::string, std::pair<std::string, std::string>> _properties;

    std::map<std::string, std::string> _types;
};

template <typename T>
T IsotopeDatabaseManager::property(const std::string& isotope, const std::string& property) const
{
    return _isotopes.at(isotope).at(property).as<T>();
}

} // end namespace nsx

#endif // CORE_CHEMISTRY_ISOTOPEDATABASEMANAGER_H
