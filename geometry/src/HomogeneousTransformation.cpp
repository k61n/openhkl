#include "HomogeneousTransformation.h"
#include "PrimitiveTransformation.h"

namespace SX
{

namespace Geometry
{

HomogeneousTransformation::HomogeneousTransformation() : _trans()
{
}

HomogeneousTransformation::~HomogeneousTransformation()
{
}

void HomogeneousTransformation::addTransformation(PrimitiveTransformation* trans)
{
	_trans.push_back(trans);
}

Matrix3d HomogeneousTransformation::getRotation(const std::vector<double>& parameters) const
{
	return (this->getTransformation(parameters).rotation());
}

HomMatrix HomogeneousTransformation::getTransformation(const std::vector<double>& parameters) const
{
	HomMatrix mat;
	mat.setIdentity();

	std::vector<PrimitiveTransformation*>::const_reverse_iterator it=_trans.rbegin();
	std::vector<double>::const_reverse_iterator pit=parameters.rbegin();

	for (;it!=_trans.rend();++it,++pit)
		mat.matrix() *= (*it)->getTransformation(*pit).matrix();

	return mat;
}

Vector3d HomogeneousTransformation::getTranslation(const std::vector<double>& parameters) const
{
	return (this->getTransformation(parameters).translation());
}

} // end namespace Geometry

} // end namespace SX
