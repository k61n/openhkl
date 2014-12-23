#include "Isotope.h"

namespace SX
{

namespace Chemistry
{

Isotope::Isotope()
: _name(""),
  _symbol(""),
  _element(""),
  _nProtons(0),
  _nNucleons(0),
  _nElectrons(0),
  _molarMass(0.0),
  _nuclearSpin(0.0),
  _state(""),
  _abundance(0.0),
  _halfLife(0.0),
  _stable(true),
  _bCoherent(0.0),
  _bIncoherent(0.0),
  _bPlus(0.0),
  _bMinus(0.0),
  _xsCoherent(0.0),
  _xsIncoherent(0.0),
  _xsScattering(0.0),
  _xsAbsorption(0.0)
{
}

double Isotope::getFormalCharge() const
{
	return (_nProtons-_nElectrons);
}

double Isotope::getMolarMass() const
{
	return _molarMass;
}

double Isotope::getNNeutrons() const
{
	return (_nNucleons-_nProtons);
}

double Isotope::getNNucleons() const
{
	return _nNucleons;
}

double Isotope::getNElectrons() const
{
	return _nElectrons;
}

double Isotope::getNProtons() const
{
	return _nProtons;
}

bool Isotope::isIon() const
{
	return (_nProtons!=_nElectrons);
}

bool Isotope::isAnion() const
{
	return (_nProtons<_nElectrons);
}

bool Isotope::isCation() const
{
	return (_nProtons>_nElectrons);
}

void Isotope::print(std::ostream& os) const
{
	os<<"Isotope "<<_name<<" (A="<<getNNucleons()<<",Q="<<getFormalCharge()<<")";
}

std::ostream& operator<<(std::ostream& os,const Isotope& isotope)
{
	isotope.print(os);
	return os;
}

} // end namespace Chemistry

} // end namespace SX
