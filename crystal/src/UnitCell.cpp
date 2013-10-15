#include "UnitCell.h"
#include <cmath>
#include "Units.h"
#include <boost/test/floating_point_comparison.hpp>

namespace SX
{

namespace Crystal
{

using SX::Units::deg;

boost::test_tools::close_at_tolerance<double> lat_tol(boost::test_tools::percent_tolerance(1e-6));

UnitCell::UnitCell():a(1),b(1),c(1),alpha(90*deg),beta(90*deg),gamma(90*deg)
{
	recalculateAll();
}

UnitCell::UnitCell(double _a, double _b, double _c, double _alpha, double _beta, double _gamma):
a(_a),b(_b),c(_c),alpha(_alpha),beta(_beta),gamma(_gamma)
{
	recalculateAll();
}
UnitCell::UnitCell(const UnitCell& rhs)
{
	a=rhs.a;b=rhs.b;c=rhs.c;
	alpha=rhs.alpha;beta=rhs.beta;gamma=rhs.gamma;
	recalculateAll();
}
UnitCell& UnitCell::operator=(const UnitCell& rhs)
{
	if (this!=&rhs)
	{
		a=rhs.a;b=rhs.b;c=rhs.c;
		alpha=rhs.alpha;beta=rhs.beta;gamma=rhs.gamma;
		recalculateAll();
	}
	return *this;
}
UnitCell::~UnitCell()
{
}
void UnitCell::set(double _a, double _b, double _c, double _alpha, double _beta, double _gamma)
{
	if (_a<0 || _b<0 || _c<0 || _alpha<0 || _alpha>M_PI || _beta<0 || beta>M_PI || _gamma<0 || _gamma>M_PI)
		throw std::range_error("UnitCell::set, some cell parameter(s) invalid(s)");
	if (_a<1e-6 || _b<1e-6 || _c<1e-6 || _alpha<1e-6 || _beta<1e-6 || _gamma<1e-6)
		throw std::range_error("UnitCell::set, some cell parameter(s) too close to zero (Maybe missing?)");
		a=_a;b=_b;c=_c;
	alpha=_alpha;beta=_beta;gamma=_gamma;
	recalculateAll();

}
void UnitCell::setA(double _a)
{
	if (_a<0 || _a<1e-6)
		throw std::range_error("UnitCell::seta, a<0 or too small");
	a=_a;
	recalculateAll();
}
void UnitCell::setB(double _b)
{
	if (_b<0 || _b<1e-6)
		throw std::range_error("UnitCell::setb, b<0 or too small");
	b=_b;
	recalculateAll();
}
void UnitCell::setC(double _c)
{
	if (_c<0 || _c<1e-6)
		throw std::range_error("UnitCell::setc, c<0 or too small");
	c=_c;
	recalculateAll();
}
void UnitCell::setAlpha(double _alpha)
{
	if (_alpha<0 || _alpha>M_PI)
		throw std::range_error("UnitCell::setalpha, alpha<0 or alpha>180");
	alpha=_alpha;
	recalculateAll();
}
void UnitCell::setBeta(double _beta)
{
	if (_beta<0 || _beta>M_PI)
		throw std::range_error("UnitCell::setalpha, beta<0 or beta>180");
	beta=_beta;
	recalculateAll();
}
void UnitCell::setGamma(double _gamma)
{
	if (_gamma<0 || _gamma>M_PI)
		throw std::range_error("UnitCell::setgamma, gamma<0 or gamma>180");
	gamma=_gamma;
	recalculateAll();
}
double UnitCell::getA() const
{
	return a;
}
double UnitCell::getB() const
{
	return b;
}
double UnitCell::getC() const
{
	return c;
}
double UnitCell::getAlpha() const
{
	return alpha;
}
double UnitCell::getBeta() const
{
	return beta;
}
double UnitCell::getGamma() const
{
	return gamma;
}
double UnitCell::volume() const
{
	return vol;
}
const Matrix33<double>& UnitCell::AMatrix() const
{
	return A;
}
const Matrix33<double>& UnitCell::BMatrix() const
{
	return B;
}
const Matrix33<double>& UnitCell::TMatrix() const
{
	return T;
}
void UnitCell::calculateVolume()
{
	vol=a*b*c*sqrt(1.0-ca*ca-cb*cb-cc*cc+2.0*ca*cb*cc);
}
void UnitCell::calculateReciprocalParameters()
{
	astar=b*c*sa/vol;
	bstar=a*c*sb/vol;
    cstar=a*b*sc/vol;
    alphastar=(cb*cc-ca)/(sb*sc);
    betastar=(ca-cc*cb)/(sa*sc);
    gammastar=(cb*ca-cc)/(sb*sa);
    alphastar=acos(alphastar);
    betastar=acos(betastar);
    gammastar=acos(gammastar);
}
void UnitCell::calculateAMatrix()
{
	double a32=c/sc*(ca-cb*cc);
	double a33=vol/(a*b*sc);
	A.set(a,b*cc,c*cb,0,b*sc,a32,0,0,a33);
}
void UnitCell::calculateTMatrix()
{
	double a32=1.0/sc*(ca-cb*cc);
	double a33=vol/(a*b*c*sc);
	T.set(1.0,cc,cb,0,sc,a32,0,0,a33);
}
void UnitCell::calculateBMatrix()
{
	B=A;
	B.invert();
	B.transposeInplace();
	return;
}
void UnitCell::calculateGTensors()
{

}
void UnitCell::calculatesincos()
{
	ca=cos(alpha);cb=cos(beta);cc=cos(gamma);
	sa=sin(alpha);sb=sin(beta);sc=sin(gamma);
	return;
}
void UnitCell::recalculateAll()
{
	calculatesincos();
	calculateVolume();
	calculateReciprocalParameters();
	calculateAMatrix();
	calculateBMatrix();
	calculateTMatrix();
	return;
}
void UnitCell::printSelf(std::ostream& os) const
{
	os << "CELL " << a << " "<<  b << " " << c << " " <<
	alpha/deg << " " << beta/deg << " " << gamma/deg;
}
void UnitCell::printDebug(std::ostream& os) const
{
	os <<"Unit Cell information :" << std::endl;
	os <<"Lattice parameters: "<< a << " " << b << " " << c << " "
											   << alpha/deg << " " << beta/deg << " " << gamma/deg <<std::endl;
	os <<"Reciprocal Lattice parameters: "<< astar << " " << bstar << " " << cstar << " "
												   << alphastar/deg << " " << betastar/deg << " " << gammastar/deg <<std::endl;
}
void UnitCell::read(std::istream& is)
{
	is >> a >> b >> c >> alpha >> beta >> gamma;
	alpha*=deg;
	beta*=deg;
	gamma*=deg;
    recalculateAll();
	return;
}
void UnitCell::transformA(V3D& vect) const
{
	A.transformV(vect);
}
void UnitCell::transformB(V3D& vect) const
{
	B.transformV(vect);
}
void UnitCell::transformT(V3D& vect) const
{
	T.transformV(vect);
}
V3D UnitCell::convertB(const V3D& vect)
{
	V3D temp(vect);
	temp=B*temp;
	return temp;
}
V3D UnitCell::convertT(const V3D& vect)
{
	V3D temp(vect);
	temp=T*temp;
	return temp;
}
V3D UnitCell::convertA(const V3D& vect)
{
	V3D temp(vect);
	temp=A*temp;
	return temp;
}

std::ostream& operator<<(std::ostream& os,const UnitCell& uc)
{
	uc.printSelf(os);
	return os;
}
std::istream& operator>>(std::istream& is, UnitCell& uc)
{
	uc.read(is);
	return is;
}

}
}
