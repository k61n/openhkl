#pragma once

#include <complex>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>

#include <yaml-cpp/yaml.h>

#include "ChemistryTypes.h"
#include "Singleton.h"
#include "Variant.h"

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
