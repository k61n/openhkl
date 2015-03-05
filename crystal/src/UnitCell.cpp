#include <cmath>
#include <stdexcept>

#include "UnitCell.h"
#include "Units.h"
#include <iomanip>

namespace SX
{

namespace Crystal
{

UnitCell::UnitCell():SX::Geometry::Basis(),_centring(LatticeCentring::P),_bravaisType(BravaisType::Triclinic)
{
}


UnitCell::UnitCell(double a, double b, double c, double alpha, double beta, double gamma, LatticeCentring centring,BravaisType bravais,std::shared_ptr<SX::Geometry::Basis> reference)
:_centring(centring),_bravaisType(bravais)
{
	double ca=cos(alpha), cb=cos(beta), cc=cos(gamma), sc=sin(gamma);
    double a32=c/sin(gamma)*(ca-cb*cc);
	double volume=a*b*c*sqrt(1.0-ca*ca-cb*cb-cc*cc+2.0*ca*cb*cc);
	double a33=volume/(a*b*sc);
	_A <<  a,b*cc,c*cb,
	       0,b*sc,a32,
	       0,0   ,a33;
	_B=_A.inverse();
	SX::Geometry::Basis::_reference=reference;
}

UnitCell::UnitCell(const UnitCell& rhs) : SX::Geometry::Basis(rhs)
{
	_centring=rhs._centring;
	_bravaisType=rhs._bravaisType;
}

UnitCell& UnitCell::operator=(const UnitCell& rhs)
{
	if (this!=&rhs)
	{
		_A=rhs._A;
		_B=rhs._B;
		_reference=rhs._reference;
		_hasSigmas=rhs._hasSigmas;
		if (_hasSigmas)
		{
			_Acov = rhs._Acov;
			_Bcov = rhs._Bcov;
		}
		_centring=rhs._centring;
		_bravaisType=rhs._bravaisType;
	}
	return *this;
}

UnitCell::UnitCell(const Eigen::Vector3d& v1,const Eigen::Vector3d& v2,const Eigen::Vector3d& v3, LatticeCentring centring,BravaisType bravais,std::shared_ptr<SX::Geometry::Basis> reference)
:SX::Geometry::Basis(v1,v2,v3,reference),_centring(centring),_bravaisType(bravais)
{

}
void UnitCell::setParams(double a, double b, double c, double alpha, double beta, double gamma)
{
	double ca=cos(alpha), cb=cos(beta), cc=cos(gamma), sc=sin(gamma);
	double a32=c/sin(gamma)*(ca-cb*cc);
	double volume=a*b*c*sqrt(1.0-ca*ca-cb*cb-cc*cc+2.0*ca*cb*cc);
	double a33=volume/(a*b*sc);
	_A <<  a,b*cc,c*cb,
		   0,b*sc,a32,
		   0,0   ,a33;
	_B=_A.inverse();
}

UnitCell UnitCell::fromDirectVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c, LatticeCentring centring, BravaisType bravais,std::shared_ptr<SX::Geometry::Basis> reference)
{
	if (coplanar(a,b,c))
		throw std::runtime_error("Class UnitCell: the direct vectors are coplanar.");
	return UnitCell(a,b,c,centring,bravais,reference);
}
	//! Build a basis from a set of three reciprocal vectors.
UnitCell UnitCell::fromReciprocalVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c,LatticeCentring centring, BravaisType bravais,std::shared_ptr<SX::Geometry::Basis> reference)
{
	if (coplanar(a,b,c))
			throw std::runtime_error("Class UnitCell: the reciprocal vectors are coplanar.");
	double rVolume = std::abs(a.dot(b.cross(c)));

	Vector3d av=b.cross(c)/rVolume;
	Vector3d bv=c.cross(a)/rVolume;
	Vector3d cv=a.cross(b)/rVolume;

	return UnitCell(av,bv,cv,centring,bravais,reference);

}

UnitCell::~UnitCell()
{
}

void UnitCell::setLatticeCentring(LatticeCentring centring)
{
	_centring=centring;
}

void UnitCell::setBravaisType(BravaisType bravais)
{
	_bravaisType=bravais;
}
std::string UnitCell::getBravaisTypeSymbol() const
{
	std::ostringstream os;
	os << static_cast<char>(_bravaisType) << static_cast<char>(_centring);
	return os.str();
}

void UnitCell::getUB(const Peak3D& p1, const Peak3D& p2)
{
	// Get Q1 and Q2 in the diffractometer basis
	auto q1=p1.getQ();
	auto q2=p2.getQ();
	auto q3=q1.cross(q2);
	q1.normalize();
	q3.normalize();
	q2=q3.cross(q1);

	//
	auto q1prime=this->toReciprocalStandard(p1.getMillerIndices());
	auto q2prime=this->toReciprocalStandard(p2.getMillerIndices());
	//
	auto q3prime=q1prime.cross(q2prime);
	q1prime.normalize();
	q3prime.normalize();
	q2prime=q3prime.cross(q1prime);
	//
	Eigen::Matrix3d ref1;

}

Eigen::Matrix3d UnitCell::getBusingLevyB() const
{
	Eigen::Matrix3d B;
	double b1=getReciprocalA();
	double b2=getReciprocalB();
	double b3=getReciprocalC();
	double c=getC();
	double beta2=getReciprocalBeta();
	double beta3=getReciprocalGamma();
	double alpha1=getAlpha();
	B <<  b1,            0,                          0,
		  b2*cos(beta3), b2*sin(beta3),              0,
		  b3*cos(beta2), -b3*sin(beta2)*cos(alpha1), 1/c;
	return B;
}

Eigen::Matrix3d UnitCell::getBusingLevyU() const
{
	return (getBusingLevyB().inverse()*_B);
}

void UnitCell::printSelf(std::ostream& os) const
{
	os << "Direct Lattice:\n";
	os << std::fixed << std::setw(10) << std::setprecision(5) << getA();
	os << std::fixed << std::setw(10) << std::setprecision(5) << getB();
	os << std::fixed << std::setw(10) << std::setprecision(5) << getC();
	os << std::fixed << std::setw(10) << std::setprecision(5) << getAlpha()/SX::Units::deg;
	os << std::fixed << std::setw(10) << std::setprecision(5) << getBeta()/SX::Units::deg;
	os << std::fixed << std::setw(10) << std::setprecision(5) << getGamma()/SX::Units::deg << std::endl;
	os << "Reciprocal Lattice:\n";
	os << std::fixed << std::setw(10) << std::setprecision(5) << getReciprocalA();
	os << std::fixed << std::setw(10) << std::setprecision(5) << getReciprocalB();
	os << std::fixed << std::setw(10) << std::setprecision(5) << getReciprocalC();
	os << std::fixed << std::setw(10) << std::setprecision(5) << getReciprocalAlpha()/SX::Units::deg;
	os << std::fixed << std::setw(10) << std::setprecision(5) << getReciprocalBeta()/SX::Units::deg;
	os << std::fixed << std::setw(10) << std::setprecision(5) << getReciprocalGamma()/SX::Units::deg << std::endl;
	os << "B matrix:" << std::endl;
	os << _B;
}

std::ostream& operator<<(std::ostream& os,const UnitCell& uc)
{
	uc.printSelf(os);
	return os;
}
std::multimap<double,Eigen::Vector3d> UnitCell::generateReflectionsInSphere(double dstarmax) const

{
	std::multimap<double,Eigen::Vector3d> map;
	// Get the bounding cube in h,k,l
	int hmax=std::ceil(dstarmax/_B.row(0).norm());
	int kmax=std::ceil(dstarmax/_B.row(1).norm());
	int lmax=std::ceil(dstarmax/_B.row(2).norm());

	// Iterate over the cuve and insert element in the map if dstar is not exceeded
	for (int h=-hmax;h<=hmax;++h)
	{
		for (int k=-kmax;k<=kmax;++k)
		{
			for (int l=-lmax;l<=lmax;++l)
			{
				auto q=toReciprocalStandard(Eigen::Vector3d(h,k,l));
				double norm=q.norm();
				if (norm < dstarmax)
					map.insert(std::map<double,Eigen::Vector3d>::value_type(norm,Eigen::Vector3d(h,k,l)));
			}
		}
	}
	return map;
}

double UnitCell::getAngle(double h1, double k1, double l1, double h2, double k2, double l2) const
{
	return getAngle(Eigen::Vector3d(h1,k1,l1),Eigen::Vector3d(h2,k2,l2));
}
double UnitCell::getAngle(const Eigen::Vector3d& hkl1, const Eigen::Vector3d& hkl2) const
{
	auto q1=toReciprocalStandard(hkl1);
	auto q2=toReciprocalReference(hkl2);
	double value=q1.dot(q2)/q1.norm()/q2.norm();
	if (value<-1)
		return M_PI;
	else if (value>1)
		return 0;
	else
		return acos(q1.dot(q2)/q1.norm()/q2.norm());
}


}
}
