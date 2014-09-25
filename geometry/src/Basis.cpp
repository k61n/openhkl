#include <stdexcept>

#include "Basis.h"

namespace SX
{

namespace Geometry
{

Basis::Basis():_A(Eigen::Matrix3d::Identity()),_B(Eigen::Matrix3d::Identity()),_reference(nullptr)
{
}
Basis::Basis(const Vector3d& a, const Vector3d& b, const Vector3d& c, ptrBasis reference): _reference(reference)
{

	if (coplanar(a,b,c))
		throw std::runtime_error("The basis vectors are coplanar.");

	_A.col(0) = a;
	_A.col(1) = b;
	_A.col(2) = c;

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


Basis::~Basis()
{
}

Basis Basis::fromDirectVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c, ptrBasis reference)
{
	if (coplanar(a,b,c))
			throw std::runtime_error("Class Basis: the direct vectors are coplanar.");
	return Basis(a,b,c,reference);
}

Basis Basis::fromReciprocalVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c, ptrBasis reference)
{

	if (coplanar(a,b,c))
		throw std::runtime_error("Class Basis: the reciprocal basis vectors are coplanar.");

	double rVolume = std::abs(a.dot(b.cross(c)));

	Vector3d av=b.cross(c)/rVolume;
	Vector3d bv=c.cross(a)/rVolume;
	Vector3d cv=a.cross(b)/rVolume;

	return Basis(av,bv,cv,reference);
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

//Vector3d Basis::geta() const
//{
//	return _A.col(0);
//}

double Basis::gete1Norm() const
{
	return _A.col(0).norm();
}
double Basis::gete2Norm() const
{
	return _A.col(1).norm();
}
double Basis::gete3Norm() const
{
	return _A.col(2).norm();
}
double Basis::gete1e2Angle() const
{
	return acos(_A.col(0).dot(_A.col(1))/gete1Norm()/gete2Norm());
}
double Basis::gete2e3Angle() const
{
	return acos(_A.col(1).dot(_A.col(2))/gete2Norm()/gete3Norm());
}
double Basis::gete1e3Angle() const
{
	return acos(_A.col(0).dot(_A.col(2))/gete1Norm()/gete3Norm());
}


//const Vector3d& Basis::getb() const
//{
//	return _A.col(1);
//}
//
//const Vector3d& Basis::getc() const
//{
//	return _A.col(2);
//}
//
//const RowVector3d& Basis::getastar() const
//{
//	return _B.row(0);
//}
//
//const RowVector3d& Basis::getbstar() const
//{
//	return _B.row(1);
//}
//
//const RowVector3d& Basis::getcstar() const
//{
//	return _B.row(2);
//}

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

Vector3d Basis::fromStandard(const Vector3d& v) const
{
	if (!_reference)
		return _B*v;
	else
		return _B*_reference->fromStandard(v);
}

Vector3d Basis::toStandard(const Vector3d& v) const
{
	if (!_reference)
		return _A*v;
	else
		return _reference->toStandard(_A*v);
}

Vector3d Basis::fromReference(const Vector3d& v) const
{
	return _B*v;
}

Vector3d Basis::toReference(const Vector3d& v) const
{
	return _A*v;
}

RowVector3d Basis::fromReciprocalStandard(const RowVector3d& rv) const
{
	if (!_reference)
		return rv*_A;
	else
		return _reference->fromReciprocalStandard(rv)*_A;
}

RowVector3d Basis::toReciprocalStandard(const RowVector3d& rv) const
{
	if (!_reference)
		return rv*_B;
	else
		return _reference->toReciprocalStandard(rv*_B);
}

RowVector3d Basis::fromReciprocalReference(const RowVector3d& rv) const
{
	return rv*_A;
}

RowVector3d Basis::toReciprocalReference(const RowVector3d& rv) const
{
	return rv*_B;
}

Matrix3d Basis::getStandardM() const
{

	if (!_reference)
		return _A;
	else
		return _reference->getStandardM()*_A;
}

Matrix3d Basis::getReciprocalStandardM() const
{

	return getStandardM().inverse();
}

const Matrix3d& Basis::getReferenceM() const
{
	return _A;
}

const Matrix3d& Basis::getReciprocalReferenceM() const
{

	return _B;
}

Matrix3d Basis::getM(const Basis& other) const
{
	Matrix3d A = getStandardM();
	Matrix3d Aother = other.getStandardM();
	return Aother.inverse()*A;
}

Matrix3d Basis::getReciprocalM(const Basis& other) const
{
	return getM(other).inverse();
}

void Basis::rebaseToStandard()
{
	_A = getStandardM();
	_B = _A.inverse();
	_reference.reset();
}

void Basis::rebaseTo(std::shared_ptr<Basis> other)
{
	if (other != nullptr)
	{
		_A = getM(*other);
		_B = _A.inverse();
		_reference = other;
	}
	else
		rebaseToStandard();
}

void Basis::transform(const Matrix3d& M)
{
	_A=_A*M;
	_B=_A.inverse();
}


} // end namespace Geometry

} // end namespace SX
