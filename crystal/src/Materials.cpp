#include <stdexcept>

#include "Error.h"
#include "Materials.h"

namespace SX
{

namespace Crystal
{

Materials::Materials() : _name(), _formula(), _density(1.0)
{
}

Materials::Materials(const std::string& name) : _name(name), _formula(), _density(1.0)
{
}

Materials::Materials(const std::string& name, const std::string& formula) : _name(name), _formula(formula), _density(1.0)
{
}

Materials::Materials(const std::string& name, const std::string& formula, double density) : _name(name), _formula(formula), _density(density)
{
}

Materials::Materials(const Materials& other) : _name(other._name), _formula(other._formula), _density(other._density)
{
}

Materials::~Materials()
{
}

Materials& Materials::operator =(const Materials& other)
{
	if (this!=&other)
	{
		_name=other._name;
		_formula=other._formula;
		_density=other._density;
	}
	return *this;
}

const std::string& Materials::getName() const
{
	return _name;
}

void Materials::setName(const std::string& name)
{
	_name=name;
}

const std::string& Materials::getFormula() const
{
	return _formula;
}

void Materials::setFormula(const std::string& formula)
{
	_formula=formula;
}

double Materials::getDensity() const
{
	return _density;
}

void Materials::setDensity(double density)
{
	if (density<=0)
		throw SX::Kernel::Error<Materials>("Invalid value for density: must be a strictly positive number");

	_density=density;
}

double Materials::getMass(double volume) const
{
	return _density*volume;
}

} // end namespace Crystal

} // end namespace SX
