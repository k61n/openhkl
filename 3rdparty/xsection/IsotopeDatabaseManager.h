//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/chemistry/IsotopeDatabaseManager.h
//! @brief     Defines class IsotopeDatabaseManager
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef XSECTION_LIB_ISOTOPEDATABASEMANAGER_H
#define XSECTION_LIB_ISOTOPEDATABASEMANAGER_H

#include "ISingleton.h"
#include "Variant.h"

#include <complex>
#include <map>

namespace xsection {

using isotopeContents = std::map<std::string, double>;

using isotopeProperties =
    std::map<std::string, Variant<bool, int, double, std::complex<double>, std::string>>;

enum class ChemicalPropertyType { String = 1, Int = 2, Double = 3, Complex = 4, Bool = 5 };

//! Class to manage the YAML-based isotope database.
class IsotopeDatabaseManager : public ISingleton<IsotopeDatabaseManager, Constructor, Destructor> {

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

} // namespace xsection

#endif // XSECTION_LIB_ISOTOPEDATABASEMANAGER_H
