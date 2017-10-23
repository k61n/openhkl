#pragma once

#include <map>
#include <complex>
#include <ostream>
#include <stdexcept>
#include <string>

#include <yaml-cpp/yaml.h>

#include "ChemistryTypes.h"
#include "Singleton.h"
#include "Some.h"

namespace nsx {

class IsotopeDatabaseManager : public Singleton<IsotopeDatabaseManager,Constructor,Destructor> {

public:

    static std::map<std::string,ChemicalPropertyType> PropertyTypes;

    IsotopeDatabaseManager();

    ~IsotopeDatabaseManager()=default;

    const std::map<std::string,isotopeProperties>& isotopes() const;

    unsigned int nIsotopes() const;

    template <typename T>
    T getProperty(const std::string& isotope, const std::string& property) const;

    bool hasProperty(const std::string& isotope, const std::string& property) const;

    const std::map<std::string,std::pair<std::string,std::string>>& properties() const;

private:

    std::map<std::string,isotopeProperties> _isotopes;

    std::map<std::string,std::pair<std::string,std::string>> _properties;

    std::map<std::string,std::string> _types;
};

template <typename T>
T IsotopeDatabaseManager::getProperty(const std::string& isotope, const std::string& property) const
{
	return _isotopes.at(isotope).at(property).cast<T>();
}

} // end namespace nsx
