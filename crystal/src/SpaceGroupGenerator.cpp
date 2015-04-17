#include <cmath>

#include "AffineTransformParser.h"
#include "Error.h"
#include "SpaceGroupGenerator.h"

// This include has to be AFTER the std::string include otherwise build error
#include <boost/algorithm/string.hpp>

namespace SX
{

namespace Crystal
{

SpaceGroupGenerator::SpaceGroupGenerator(std::string generator)
{
	// The parser for generator expression
	SX::Utils::AffineTransformParser<std::string::iterator> parser;

	bool match=qi::phrase_parse(generator.begin(),generator.end(),parser,qi::blank, _symmetryOperation);
	if (!match)
		throw SX::Kernel::Error<SpaceGroupGenerator>("Invalid generator expression: "+ generator);
}

SpaceGroupGenerator::SpaceGroupGenerator(const affineTransformation& symmetryOperation) : _symmetryOperation(symmetryOperation)
{
}

SpaceGroupGenerator::SpaceGroupGenerator(const SpaceGroupGenerator& other)
: _symmetryOperation(other._symmetryOperation)
{
}

SpaceGroupGenerator& SpaceGroupGenerator::operator=(const SpaceGroupGenerator& other)
{
	if (this != &other)
		_symmetryOperation = other._symmetryOperation;
	return *this;
}

bool SpaceGroupGenerator::operator==(const SpaceGroupGenerator& other) const
{

	// If the rotation part of the symmetry operator of two generators are not the same then the two generators are not equal.
	if (_symmetryOperation.rotation()!=other._symmetryOperation.rotation())
		return false;

	// If the difference between the translation part of the symmetry operator of two generators is not a vector of integers
	// then the two generators are not equal.
	auto deltat = _symmetryOperation.translation() - other._symmetryOperation.translation();

	return ((fmod(deltat[0],1.0)==0) & (fmod(deltat[1],1.0)==0) & (fmod(deltat[2],1.0)==0));
}

SpaceGroupGenerator::~SpaceGroupGenerator()
{
}

SpaceGroupGenerator SpaceGroupGenerator::operator*(const SpaceGroupGenerator& other) const
{
	return SpaceGroupGenerator(_symmetryOperation*other._symmetryOperation);
}

const affineTransformation& SpaceGroupGenerator::getSymmetryOperation() const
{
	return _symmetryOperation;
}

void SpaceGroupGenerator::print(std::ostream& os) const
{
	os<<"Transformation matrix:"<<std::endl;
	os<<_symmetryOperation.matrix()<<std::endl;
}

std::ostream& operator<<(std::ostream& os, const SpaceGroupGenerator& sgg)
{
	sgg.print(os);
	return os;
}

} // end namespace Crystal

} // end namespace SX
