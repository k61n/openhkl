#ifndef NSXLIB_ISOTOPEDATABASEMANAGER_H
#define NSXLIB_ISOTOPEDATABASEMANAGER_H

#include <map>
#include <complex>
#include <ostream>
#include <stdexcept>
#include <string>

#include "yaml-cpp/yaml.h"

#include <boost/any.hpp>

#include "../kernel/Singleton.h"
#include "../chemistry/ChemistryTypes.h"

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

    static const std::string DatabasePath;

};

template <typename T>
T IsotopeDatabaseManager::getProperty(const std::string& isotope, const std::string& property) const
{
	return boost::any_cast<T>(_isotopes.at(isotope).at(property));
}

} // end namespace nsx

#endif // NSXLIB_ISOTOPEDATABASEMANAGER_H
