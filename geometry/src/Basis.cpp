#include <stdexcept>

#include "Basis.h"

namespace SX
{

namespace Geometry
{

Basis::Basis(const Vector3d& e1, const Vector3d& e2, const Vector3d& e3, ptrBasis reference) : _reference(reference)
{

	if (coplanar(e1,e2,e3))
		throw std::runtime_error("The basis vectors are coplanar.");

	_A.col(0) = e1;
	_A.col(1) = e2;
	_A.col(2) = e3;

	_B = _A.inverse();
}

Basis::Basis(const Basis& other)
{

	_reference = other._reference;
	_A = other._A;
	_B = other._B;
}

Basis& Basis::operator=(const Basis& other)
{
	if (this != &other)
	{
		_reference = other._reference;
		_A = other._A;
		_B = other._B;
	}
	return *this;
}

Basis Basis::fromDirectVectors(const Vector3d& e1, const Vector3d& e2, const Vector3d& e3, ptrBasis reference)
{
	return Basis(e1,e2,e3,reference);
}

Basis Basis::fromReciprocalVectors(const Vector3d& e1, const Vector3d& e2, const Vector3d& e3, ptrBasis reference)
{

	if (coplanar(e1,e2,e3))
		throw std::runtime_error("The reciprocal basis vectors are coplanar.");

	double rVolume = std::abs(e1.dot(e2.cross(e3)));

	Vector3d a=e2.cross(e3)/rVolume;
	Vector3d b=e3.cross(e1)/rVolume;
	Vector3d c=e1.cross(e2)/rVolume;

	return Basis(a,b,c,reference);
}

bool Basis::coplanar(const Vector3d& v1, const Vector3d& v2, const Vector3d& v3, double tolerance)
{
	return (std::abs(v1.dot(v2.cross(v3))) < tolerance);
}

std::ostream& operator<<(std::ostream& os,const Basis& b)
{
	os<<"Direct basis:\n"<<b._A<<std::endl<<"\nReciprocal basis:\n"<<b._B<<std::endl;

	return os;
}

Matrix3d Basis::getMetricTensor() const
{

	if (!_reference)
		return _A.transpose()*_A;
	else
		return _A.transpose()*_reference->getMetricTensor()*_A;
}

Matrix3d Basis::getReciprocalMetricTensor() const
{
	if (!_reference)
		return _B*_B.transpose();
	else
		return _B*_reference->getReciprocalMetricTensor()*_B.transpose();
}

double Basis::getFractionalVolume() const
{
	return sqrt((_A.transpose()*_A).determinant());
}

double Basis::getFractionalReciprocalVolume() const
{
	return sqrt((_B*_B.transpose()).determinant());
}

double Basis::getVolume() const
{
	Matrix3d g=getMetricTensor();
	return sqrt(g.determinant());
}

double Basis::getReciprocalVolume() const
{
	Matrix3d gs=getReciprocalMetricTensor();
	return sqrt(gs.determinant());
}

Vector3d Basis::fromStandardBasis(const Vector3d& v) const
{
	if (!_reference)
		return _B*v;
	else
		return _B*_reference->fromStandardBasis(v);
}

Vector3d Basis::toStandardBasis(const Vector3d& v) const
{
	if (!_reference)
		return _A*v;
	else
		return _reference->toStandardBasis(_A*v);
}

Vector3d Basis::fromReferenceBasis(const Vector3d& v) const
{
	return _B*v;
}

Vector3d Basis::toReferenceBasis(const Vector3d& v) const
{
	return _A*v;
}

RowVector3d Basis::fromReciprocalStandardBasis(const RowVector3d& rv) const
{
	if (!_reference)
		return rv*_A;
	else
		return _reference->fromReciprocalStandardBasis(rv)*_A;
}

RowVector3d Basis::toReciprocalStandardBasis(const RowVector3d& rv) const
{
	if (!_reference)
		return rv*_B;
	else
		return _reference->toReciprocalStandardBasis(rv*_B);
}

RowVector3d Basis::fromReciprocalReferenceBasis(const RowVector3d& rv) const
{
	return rv*_A;
}

RowVector3d Basis::toReciprocalReferenceBasis(const RowVector3d& rv) const
{
	return rv*_B;
}


Basis::~Basis() {
}

} // end namespace Geometry

} // end namespace SX
