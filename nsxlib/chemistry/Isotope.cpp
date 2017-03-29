#include <complex>
#include <stdexcept>

#include "Isotope.h"
#include "../utils/Path.h"
#include "../utils/Units.h"

namespace SX {

namespace Chemistry {

using SX::Units::UnitsManager;
using SX::Utils::Path;

std::map<std::string,Isotope::PropertyType> Isotope::PropertyTypes = {{"string",PropertyType::String},
                                                                      {"int",PropertyType::Int},
                                                                      {"double",PropertyType::Double},
                                                                      {"complex",PropertyType::Complex},
                                                                      {"bool",PropertyType::Bool}};

// Solve build failure on osx
template<>
std::string Isotope::getProperty<std::string>(const std::string& propertyName) const;

template<>
int Isotope::getProperty<int>(const std::string& propertyName) const;

template<>
double Isotope::getProperty<double>(const std::string& propertyName) const;

template<>
std::complex<double> Isotope::getProperty<std::complex<double>>(const std::string& propertyName) const;

template<>
bool Isotope::getProperty<bool>(const std::string& propertyName) const;

Isotope::Isotope(const ptree& isotopeNode)
{
    UnitsManager* um=UnitsManager::Instance();

    _name=isotopeNode.get<std::string>("<xmlattr>.name");

    for (const auto& propertyNode : isotopeNode) {
        std::string pname = propertyNode.first;

        if (pname.compare("<xmlattr>")==0)
            continue;

        const auto& pnode = propertyNode.second;
        _types[pname] = pnode.get<std::string>("<xmlattr>.type");
        _units[pname] = pnode.get<std::string>("<xmlattr>.unit","unitless");

        switch (PropertyTypes[_types[pname]]) {

        case PropertyType::String:
            _properties[pname] = pnode.get_value<std::string>();
            break;
        case PropertyType::Int:
            _properties[pname] = pnode.get_value<int>();
            break;
        case PropertyType::Double:
            _properties[pname] = pnode.get_value<double>()*um->get(_units[pname]);
            break;
        case PropertyType::Complex:
            _properties[pname] = pnode.get_value<std::complex<double>>()*um->get(_units[pname]);
            break;
        case PropertyType::Bool:
            _properties[pname] = pnode.get_value<bool>();
            break;
        default:
            throw std::runtime_error("unknown property type for "+pname+" property");
        }
    }
}

const std::string& Isotope::getName() const
{
	return _name;
}

bool Isotope::hasProperty(const std::string& propertyName) const
{
    auto it = _properties.find(propertyName);
    return (it != _properties.end());
}

void Isotope::print(std::ostream& os) const
{
	os<<"Isotope "<<_name<<" ["<<getProperty<int>("n_protons")<<","<<getProperty<int>("n_neutrons")<<"]";
}

ptree Isotope::writeToXML() const
{
    UnitsManager* um=UnitsManager::Instance();

	ptree node;
	node.put("<xmlattr>.name",_name);
	for (const auto& prop : _properties) {
		std::string pname = prop.first;
		ptree& isnode=node.add(pname,"");
		isnode.put("<xmlattr>.type",_types.at(pname));
		isnode.put("<xmlattr>.unit",_units.at(pname));

        switch (PropertyTypes[_types.at(pname)]) {

        case PropertyType::String:
    		isnode.put_value(any_cast<std::string>(prop.second));
            break;
        case PropertyType::Int:
    		isnode.put_value(any_cast<int>(prop.second));
            break;
        case PropertyType::Double:
    		isnode.put_value(any_cast<double>(prop.second)/um->get(_units.at(pname)));
            break;
        case PropertyType::Complex:
    		isnode.put_value(any_cast<std::complex<double>>(prop.second)/um->get(_units.at(pname)));
            break;
        case PropertyType::Bool:
    		isnode.put_value(any_cast<bool>(prop.second));
            break;
        }
	}
	return node;
}

std::ostream& operator<<(std::ostream& os,const Isotope& isotope)
{
	isotope.print(os);
	return os;
}

} // end namespace Chemistry

} // end namespace SX
