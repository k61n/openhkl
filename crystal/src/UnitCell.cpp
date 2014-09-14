#include <cmath>
#include "UnitCell.h"
#include "Units.h"

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
	_A << a,b*cc,c*cb,
	       0,b*sc,a32,
	       0,0   ,a33;
	_B=_A.inverse();
	SX::Geometry::Basis::_reference=reference;
}
UnitCell::UnitCell(const UnitCell& rhs)
{
	_A=rhs._A;
	_B=rhs._B;
	_reference=rhs._reference;
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
	_centring=rhs._centring;
	_bravaisType=rhs._bravaisType;
	}
	return *this;
}

UnitCell::UnitCell(const Eigen::Vector3d& v1,const Eigen::Vector3d& v2,const Eigen::Vector3d& v3, LatticeCentring centring,BravaisType bravais,std::shared_ptr<SX::Geometry::Basis> reference)
:SX::Geometry::Basis(v1,v2,v3,reference),_centring(centring),_bravaisType(bravais)
{

}

UnitCell UnitCell::fromDirectVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c, LatticeCentring centring, BravaisType bravais,std::shared_ptr<SX::Geometry::Basis> reference)
{
	if (coplanar(a,b,c))
		throw std::runtime_error("The direct vectors are coplanar.");
	return UnitCell(a,b,c,centring,bravais,reference);
}
	//! Build a basis from a set of three reciprocal vectors.
UnitCell UnitCell::fromReciprocalVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c,LatticeCentring centring, BravaisType bravais,std::shared_ptr<SX::Geometry::Basis> reference)
{
	if (coplanar(a,b,c))
			throw std::runtime_error("The reciprocal vectors are coplanar.");
	double rVolume = std::abs(a.dot(b.cross(c)));

	Vector3d av=b.cross(c)/rVolume;
	Vector3d bv=c.cross(a)/rVolume;
	Vector3d cv=a.cross(b)/rVolume;

	return UnitCell(av,bv,cv,centring,bravais,reference);

}

UnitCell::~UnitCell()
{
}

double UnitCell::getA() const
{
	return gete1Norm();
}
double UnitCell::getB() const
{
	return gete2Norm();
}
double UnitCell::getC() const
{
	return gete3Norm();
}
double UnitCell::getAlpha() const
{
	return gete2e3Angle();
}
double UnitCell::getBeta() const
{
	return gete1e3Angle();
}
double UnitCell::getGamma() const
{
	return gete1e2Angle();
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

void UnitCell::printSelf(std::ostream& os) const
{
	os << "Cell parameters: " << getA() << ", " << getB() << ", " << getC() << ", " << getAlpha()/SX::Units::deg << ", " <<getBeta()/SX::Units::deg << ", "<< getGamma()/SX::Units::deg <<std::endl;
	os << "Lattice centring: " << static_cast<char>(_centring) << std::endl;
	os << "Bravais type: "     << static_cast<char>(_bravaisType) << std::endl;

}

std::ostream& operator<<(std::ostream& os,const UnitCell& rhs)
{
	rhs.printSelf(os);
	return os;
}


}
}
