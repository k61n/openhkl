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

    class Isotope {

    public:

        Isotope()=delete;

        Isotope(const Isotope& other)=default;

        Isotope& operator=(const Isotope& other)=default;

        Isotope(const YAML::Node& isotopeNode);

        //! Destructor
        ~Isotope()=default;

        //! Returns the name of this Isotope
        const std::string& getName() const;

        template <typename T>
        T getProperty(const std::string& propertyName) const;

        bool hasProperty(const std::string& propertyName) const;

        //! Print some informations about this Isotope on a stream
        void print(std::ostream& os) const;

    private:

        //! The name of this Isotope
        std::string _name;

        std::map<std::string,boost::any> _properties;
        std::map<std::string,std::string> _units;
        std::map<std::string,std::string> _types;

    };

public:

    IsotopeDatabaseManager();

    ~IsotopeDatabaseManager()=default;

    const Isotope& getIsotope(const std::string& name) const;

    const std::map<std::string,Isotope>& database() const;

private:

    std::map<std::string,Isotope> _database;

    static const std::string DatabasePath;

};

template <typename T>
T IsotopeDatabaseManager::Isotope::getProperty(const std::string& propertyName) const
{
    auto pit = _properties.find(propertyName);
    if (pit == _properties.end())
        throw std::runtime_error("Isotope "+_name+": unknown property name ("+propertyName+")");

    return boost::any_cast<T>(pit->second);
}

//! Overloads the operator<< with an Isotope object
std::ostream& operator<<(std::ostream&,const IsotopeDatabaseManager::Isotope&);

} // end namespace nsx

#endif // NSXLIB_ISOTOPEDATABASEMANAGER_H
