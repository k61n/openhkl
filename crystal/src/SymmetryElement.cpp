#include <cmath>

#include "AffineTransformParser.h"
#include "Error.h"
#include "SymmetryElement.h"

// This include has to be AFTER the std::string include otherwise build error
#include <boost/algorithm/string.hpp>

namespace SX
{

namespace Crystal
{

SymmetryElement::SymmetryElement(std::string generator)
{
	// The parser for generator expression
	SX::Utils::AffineTransformParser<std::string::iterator> parser;

	bool match=qi::phrase_parse(generator.begin(),generator.end(),parser,qi::blank, _symmetryOperation);
	if (!match)
		throw SX::Kernel::Error<SymmetryElement>("Invalid generator expression: "+ generator);
}

SymmetryElement::SymmetryElement(const affineTransformation& symmetryOperation) : _symmetryOperation(symmetryOperation)
{
}

SymmetryElement::SymmetryElement(const SymmetryElement& other)
: _symmetryOperation(other._symmetryOperation)
{
}

SymmetryElement& SymmetryElement::operator=(const SymmetryElement& other)
{
	if (this != &other)
		_symmetryOperation = other._symmetryOperation;
	return *this;
}

bool SymmetryElement::operator==(const SymmetryElement& other) const
{

	// If the rotation part of the symmetry operator of two generators are not the same then the two generators are not equal.
	if (_symmetryOperation.linear()!=other._symmetryOperation.linear())
		return false;

	// If the difference between the translation part of the symmetry operator of two generators is not a vector of integers
	// then the two generators are not equal.
	auto deltat = _symmetryOperation.translation() - other._symmetryOperation.translation();

	return ((std::abs(std::remainder(deltat[0],1.0))<=1.0e-9) &&
			 (std::abs(std::remainder(deltat[1],1.0))<=1.0e-9) &&
			 (std::abs(std::remainder(deltat[2],1.0))<=1.0e-9));
}

SymmetryElement::~SymmetryElement()
{
}

SymmetryElement SymmetryElement::operator*(const SymmetryElement& other) const
{
	SymmetryElement sym(_symmetryOperation*other._symmetryOperation);

	sym._symmetryOperation(0,3) = std::remainder(sym._symmetryOperation(0,3),1.0);
	sym._symmetryOperation(1,3) = std::remainder(sym._symmetryOperation(1,3),1.0);
	sym._symmetryOperation(2,3) = std::remainder(sym._symmetryOperation(2,3),1.0);

	return sym;
}

const affineTransformation& SymmetryElement::getSymmetryOperation() const
{
	return _symmetryOperation;
}

void SymmetryElement::print(std::ostream& os) const
{
	os<<"Transformation matrix:"<<std::endl;
	os<<_symmetryOperation.matrix()<<std::endl;
}

std::ostream& operator<<(std::ostream& os, const SymmetryElement& sym)
{
	sym.print(os);
	return os;
}

} // end namespace Crystal

} // end namespace SX
