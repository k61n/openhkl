#include <stdexcept>

#include "Basis.h"

namespace SX
{

namespace Geometry
{

Basis::Basis():_A(Eigen::Matrix3d::Identity()),_B(Eigen::Matrix3d::Identity()),_reference(nullptr), _Asigmas(nullptr), _Bsigmas(nullptr), _hasSigmas(false)
{
}

Basis::Basis(const Vector3d& a, const Vector3d& b, const Vector3d& c, ptrBasis reference): _reference(reference),_Asigmas(nullptr),_Bsigmas(nullptr)
{

	if (coplanar(a,b,c))
		throw std::runtime_error("The basis vectors are coplanar.");

	_A.col(0) = a;
	_A.col(1) = b;
	_A.col(2) = c;

	_B = _A.inverse();

	if (reference)
	{
		_hasSigmas = reference->_hasSigmas;
		if (reference->hasSigmas())
		{
			_Asigmas = new Matrix3d(*(reference->_Asigmas));
			_Bsigmas = new Matrix3d();
			propagateSigmas(_A);
		}
	}
	else
		_hasSigmas=false;
}

Basis::Basis(const Basis& other)
{

	_reference = other._reference;
	_A = other._A;
	_B = other._B;

	if (_reference)
	{
		_hasSigmas = _reference->_hasSigmas;
		_Asigmas = new Matrix3d(*(_reference->_Asigmas));
		_Bsigmas = new Matrix3d(*(_reference->_Bsigmas));
	}
	else
		_hasSigmas = false;

}

Basis& Basis::operator=(const Basis& other)
{
	if (this != &other)
	{
		_reference = other._reference;
		_A = other._A;
		_B = other._B;

		if (_reference)
		{
			_hasSigmas = _reference->_hasSigmas;
			_Asigmas = new Matrix3d(*(_reference->_Asigmas));
			_Bsigmas = new Matrix3d(*(_reference->_Bsigmas));
		}
		else
			_hasSigmas = false;

	}
	return *this;
}


Basis::~Basis()
{
	if (_Asigmas)
		delete _Asigmas;

	if (_Bsigmas)
		delete _Bsigmas;
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

void Basis::rebaseTo(std::shared_ptr<Basis> other,bool sigmasFromReference)
{
	if (other != nullptr)
	{
		_A = getM(*other);
		_B = _A.inverse();
		_reference = other;

		if (_reference->hasSigmas() && sigmasFromReference)
		{
			if (!hasSigmas())
			{
				_hasSigmas = true;
				_Asigmas = new Matrix3d(*(_reference->_Asigmas));
				_Bsigmas = new Matrix3d();
			}
			propagateSigmas(_A);
		}
	}
	else
		rebaseToStandard();
}

void Basis::transform(const Matrix3d& M)
{
	_A=_A*M;
	_B=_A.inverse();

	if (hasSigmas())
	{
		*(_Asigmas) = (((*_Asigmas).cwiseProduct(*_Asigmas))*(M.cwiseProduct(M))).cwiseSqrt();
		calculateSigmasDirectToReciprocal(true);
	}
}

void Basis::propagateSigmas(const Matrix3d& P)
{
	(*_Asigmas) = (((*_Asigmas).cwiseProduct((*_Asigmas)))*(P.cwiseProduct(P))).cwiseSqrt();
	calculateSigmasDirectToReciprocal(true);
}

bool Basis::hasSigmas() const
{
	return _hasSigmas;
}

void Basis::calculateSigmasDirectToReciprocal(bool direction)
{

	// For uncorrelated errors, given a matrix A with inverse A-1,
	// then the uncertainty squared [sigmaA-1]^2_{alpha,beta}=[A-1]^2_{alpha,i}.[sigmaA]^2_{i,j}.[A-1]^2_{j,beta}
	// Here, use the fact that A^-1=B, or B^-1=A when converting from direct to reciprocal or reciprocal to direct
	// Explained in M. Lefebvre, R.K. Keeler, R. Sobie, J. White, Propagation of errors for matrix inversion,
	// Nuclear Instruments and Methods in Physics Research Section A: Accelerators, Spectrometers, Detectors and Associated Equipment, Volume 451, Issue 2, 1 September 2000, Pages 520-528

	Eigen::Matrix3d input, inputS, *outputS;
	if (direction)
	{
		input   = getReciprocalStandardM();
		inputS  = *_Asigmas;
		outputS = _Bsigmas;
	}
	else
	{
		input   = getStandardM();
		inputS  = *_Bsigmas;
		outputS = _Asigmas;
	}

	// Calculate [A-1]^2
	input=input.cwiseProduct(input);
	// Calculate [sigmaA]^2
	inputS=inputS.cwiseProduct(inputS);

	for (int i=0;i<3;++i)
	{
		for (int j=0;j<3;++j)
		{
			(*outputS)(i,j)=sqrt(input.row(i)*inputS*input.col(j));
		}
	}

}

void Basis::setReciprocalSigmas(const Eigen::Vector3d& sas,const Eigen::Vector3d& sbs,const Eigen::Vector3d& scs)
{
	if (!hasSigmas())
	{
		_Asigmas = new Matrix3d();
		_Bsigmas = new Matrix3d();
	}
	_Bsigmas->row(0) = sas;
	_Bsigmas->row(1) = sbs;
	_Bsigmas->row(2) = scs;

	calculateSigmasDirectToReciprocal(false);

	_hasSigmas = true;
}

void Basis::setReciprocalSigmas(const Matrix3d& sigmas)
{
	if (!hasSigmas())
	{
		_Asigmas = new Matrix3d();
		_Bsigmas = new Matrix3d();
	}

	*_Bsigmas = sigmas;

	calculateSigmasDirectToReciprocal(false);

	_hasSigmas = true;
}

void Basis::setDirectSigmas(const Eigen::Vector3d& sa,const Eigen::Vector3d& sb,const Eigen::Vector3d& sc)
{
	if (!hasSigmas())
	{
		_Asigmas = new Matrix3d();
		_Bsigmas = new Matrix3d();
	}
	_Asigmas->col(0) = sa;
	_Asigmas->col(1) = sb;
	_Asigmas->col(2) = sc;

	calculateSigmasDirectToReciprocal(true);

	_hasSigmas = true;

}

void Basis::setDirectSigmas(const Matrix3d& sigmas)
{
	if (!hasSigmas())
	{
		_Asigmas = new Matrix3d();
		_Bsigmas = new Matrix3d();
	}

	*_Asigmas = sigmas;

	calculateSigmasDirectToReciprocal(true);

	_hasSigmas = true;
}

Matrix3d  Basis::getDirectSigmas()
{
	if (_hasSigmas)
		return *_Asigmas;
	else
		return Matrix3d::Zero();
}

Matrix3d  Basis::getReciprocalSigmas()
{
	if (_hasSigmas)
		return *_Bsigmas;
	else
		return Matrix3d::Zero();

}

} // end namespace Geometry

} // end namespace SX
