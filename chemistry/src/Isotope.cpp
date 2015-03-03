#include "Isotope.h"

namespace SX
{

namespace Chemistry
{

Isotope* Isotope::create(const std::string& name)
{
	return (new Isotope(name));
}

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

Isotope::Isotope(const std::string& name)
: _name(name),
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

Isotope::~Isotope()
{
}

bool Isotope::operator==(const Isotope& other)
{
	// Two isotopes is1 and is2 are considered to be equal if Z(is1)=Z(is2) and A(is1)=A(is2)
	return ((_nProtons==other._nProtons) && (_nNucleons==other._nNucleons));
}

const std::string& Isotope::getName() const
{
	return _name;
}

const std::string& Isotope::getSymbol() const
{
	return _symbol;
}

const std::string& Isotope::getState() const
{
	return _state;
}

double Isotope::getNuclearSpin() const
{
	return _nuclearSpin;
}

double Isotope::getAbundance() const
{
	return _abundance;
}

double Isotope::getHalfLife() const
{
	return _halfLife;
}

bool Isotope::getStable() const
{
	return _stable;
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

std::complex<double>  Isotope::getBCoherent() const
{
	return _bCoherent;
}

std::complex<double>  Isotope::getBIncoherent() const
{
	return _bIncoherent;
}

std::complex<double>  Isotope::getBPlus() const
{
	return _bPlus;
}

std::complex<double>  Isotope::getBMinus() const
{
	return _bMinus;
}

double Isotope::getXsCoherent() const
{
	return _xsCoherent;
}

double Isotope::getXsIncoherent() const
{
	return _xsIncoherent;
}

double Isotope::getXsAbsorption() const
{
	return _xsAbsorption;
}

double Isotope::getXsScattering() const
{
	return _xsScattering;
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
	os<<"Isotope "<<_name<<" ["<<_nProtons<<","<<_nNucleons<<"]";
}

std::ostream& operator<<(std::ostream& os,const Isotope& isotope)
{
	isotope.print(os);
	return os;
}

} // end namespace Chemistry

} // end namespace SX
