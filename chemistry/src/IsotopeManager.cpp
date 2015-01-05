#include "Isotope.h"
#include "IsotopeManager.h"

namespace SX
{

namespace Chemistry
{

isotopeMap IsotopeManager::_isotopeRegistry=isotopeMap();

std::string IsotopeManager::database="SXIsotopeDatabase.xml";

IsotopeManager::IsotopeManager()
{
}

unsigned int IsotopeManager::getNRegisteredIsotopes() const
{
	return _isotopeRegistry.size();
}

Isotope* IsotopeManager::getIsotope(const std::string& name) const
{
	auto p=_isotopeRegistry.find(name);

	if (p!=_isotopeRegistry.end())
		return p->second;
	else
	{
		ptree root;
		read_xml(database,root);

		BOOST_FOREACH(ptree::value_type const& v, root.get_child("sx_isotope_database"))
		{
			if (v.first.compare("isotope")!=0)
				continue;

			if (v.second.get<std::string>("<xmlattr>.name").compare(name)==0)
			{
				Isotope* is=new Isotope();
				is->_name=name;
				is->_symbol=v.second.get<std::string>("symbol");
				is->_element=v.second.get<std::string>("element");
				is->_nProtons=v.second.get<int>("nProtons");
				is->_nNucleons=v.second.get<int>("nNucleons");
				is->_nElectrons=is->_nProtons;
				is->_molarMass=v.second.get<double>("molarMass");
				is->_nuclearSpin=v.second.get<double>("nuclearSpin");
				is->_state=v.second.get<std::string>("state");
				is->_abundance=v.second.get<double>("abundance",0.0);
				is->_halfLife=v.second.get<double>("halfLife",std::numeric_limits<double>::infinity());
				is->_stable=v.second.get<bool>("stable");
				is->_bCoherent=v.second.get<std::complex<double>>("bCoherent");
				is->_bIncoherent=v.second.get<std::complex<double>>("bIncoherent");
				is->_bPlus=v.second.get<std::complex<double>>("bPlus");
				is->_bMinus=v.second.get<std::complex<double>>("bMinus");
				is->_xsCoherent=v.second.get<double>("xsCoherent");
				is->_xsIncoherent=v.second.get<double>("xsIncoherent");
				is->_xsScattering=v.second.get<double>("xsScattering");
				is->_xsAbsorption=v.second.get<double>("xsAbsorption");
				auto ret=_isotopeRegistry.insert(isotopePair(name,is));
				return ret.first->second;
			}
		}
		throw SX::Kernel::Error<IsotopeManager>("No match for entry "+name);
	}
}

} // end namespace Chemistry

} // end namespace SX
