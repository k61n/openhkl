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

UnitCell::UnitCell():_a(1),_b(1),_c(1),_alpha(90*deg),_beta(90*deg),_gamma(90*deg)
{
	recalculateAll();
}

UnitCell::UnitCell(double a, double b, double c, double alpha, double beta, double gamma, Centring type):
_a(a),_b(b),_c(c),_alpha(alpha),_beta(beta),_gamma(gamma),_type(type)
{
	recalculateAll();
}
UnitCell::UnitCell(const UnitCell& rhs)
{
	_a=rhs._a;_b=rhs._b;_c=rhs._c;
	_alpha=rhs._alpha;_beta=rhs._beta;_gamma=rhs._gamma;
	_type=rhs._type;
	recalculateAll();
}
UnitCell& UnitCell::operator=(const UnitCell& rhs)
{
	if (this!=&rhs)
	{
		_a=rhs._a;_b=rhs._b;_c=rhs._c;
		_alpha=rhs._alpha;_beta=rhs._beta;_gamma=rhs._gamma;
		_type=rhs._type;
		recalculateAll();
	}
	return *this;
}
UnitCell::~UnitCell()
{
}
void UnitCell::setCell(double a, double b, double c, double alpha, double beta, double gamma)
{
	if (a<0 || b<0 || c<0 || alpha<0 || alpha>M_PI || beta<0 || beta>M_PI || gamma<0 || gamma>M_PI)
		throw std::range_error("UnitCell::set, some cell parameter(s) invalid(s)");
	if (a<1e-6 || b<1e-6 || c<1e-6 || alpha<1e-6 || beta<1e-6 || gamma<1e-6)
		throw std::range_error("UnitCell::set, some cell parameter(s) too close to zero (Maybe missing?)");
	_a=a;_b=b;_c=c;
	_alpha=alpha;_beta=beta;_gamma=gamma;
	recalculateAll();

}
void UnitCell::setCentringType(Centring type)
{
	_type=type;
}

void UnitCell::setA(double a)
{
	if (a<1e-6)
		throw std::range_error("UnitCell::seta, a<0 or too small");
	_a=a;
	recalculateAll();
}
void UnitCell::setB(double b)
{
	if (_b<1e-6)
		throw std::range_error("UnitCell::setb, b<0 or too small");
	_b=b;
	recalculateAll();
}
void UnitCell::setC(double c)
{
	if (c<1e-6)
		throw std::range_error("UnitCell::setc, c<0 or too small");
	_c=c;
	recalculateAll();
}
void UnitCell::setAlpha(double alpha)
{
	if (alpha<0 || alpha>M_PI)
		throw std::range_error("UnitCell::setalpha, alpha<0 or alpha>180");
	_alpha=alpha;
	recalculateAll();
}
void UnitCell::setBeta(double beta)
{
	if (beta<0 || beta>M_PI)
		throw std::range_error("UnitCell::setalpha, beta<0 or beta>180");
	_beta=beta;
	recalculateAll();
}
void UnitCell::setGamma(double gamma)
{
	if (gamma<0 || gamma>M_PI)
		throw std::range_error("UnitCell::setgamma, gamma<0 or gamma>180");
	_gamma=gamma;
	recalculateAll();
}
double UnitCell::getA() const
{
	return _a;
}
double UnitCell::getB() const
{
	return _b;
}
double UnitCell::getC() const
{
	return _c;
}
double UnitCell::getAlpha() const
{
	return _alpha;
}
double UnitCell::getBeta() const
{
	return _beta;
}
double UnitCell::getGamma() const
{
	return _gamma;
}
UnitCell::Centring UnitCell::getType() const
{
	return _type;
}
double UnitCell::getVolume() const
{
	return _volume;
}
const Eigen::Matrix3d& UnitCell::getAMatrix() const
{
	return _A;
}
const Eigen::Matrix3d& UnitCell::getBMatrix() const
{
	return _B;
}
const Eigen::Matrix3d& UnitCell::getTMatrix() const
{
	return _T;
}
const Eigen::Matrix3d& UnitCell::getMetricTensor() const
{
	return _G;
}
void UnitCell::calculateVolume()
{
	_volume=_a*_b*_c*sqrt(1.0-_ca*_ca-_cb*_cb-_cc*_cc+2.0*_ca*_cb*_cc);
}
void UnitCell::calculateReciprocalParameters()
{
	_astar=_b*_c*_sa/_volume;
	_bstar=_a*_c*_sb/_volume;
    _cstar=_a*_b*_sc/_volume;
    _alphastar=(_cb*_cc-_ca)/(_sb*_sc);
    _betastar=(_ca-_cc*_cb)/(_sa*_sc);
    _gammastar=(_cb*_ca-_cc)/(_sb*_sa);
    _alphastar=acos(_alphastar);
    _betastar=acos(_betastar);
    _gammastar=acos(_gammastar);
}
void UnitCell::calculateAMatrix()
{
	double a32=_c/_sc*(_ca-_cb*_cc);
	double a33=_volume/(_a*_b*_sc);
	_A << _a,_b*_cc,_c*_cb,
		   0,_b*_sc,a32,
		   0,0     ,a33;
}
void UnitCell::calculateTMatrix()
{
	double a32=1.0/_sc*(_ca-_cb*_cc);
	double a33=_volume/(_a*_b*_c*_sc);
	_T << 1.0,_cc,_cb,0,_sc,a32,0,0,a33;
}
void UnitCell::calculateBMatrix()
{
	_B=_A.inverse();
	_B.transposeInPlace();
	return;
}
void UnitCell::calculateGTensors()
{
	_G=_A*_A.transpose();
}
void UnitCell::calculatesincos()
{
	_ca=cos(_alpha);_cb=cos(_beta);_cc=cos(_gamma);
	_sa=sin(_alpha);_sb=sin(_beta);_sc=sin(_gamma);
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
	calculateGTensors();
	return;
}
void UnitCell::printSelf(std::ostream& os) const
{
	os << "CELL " << _a << " "<<  _b << " " << _c << " " <<
	_alpha/deg << " " << _beta/deg << " " << _gamma/deg;
}

void UnitCell::read(std::istream& is)
{
	is >> _a >> _b >> _c >> _alpha >> _beta >> _gamma;
	_alpha*=deg;
	_beta*=deg;
	_gamma*=deg;
    recalculateAll();
	return;
}
void UnitCell::transformA(Eigen::Vector3d& vect) const
{
	vect=_A*vect;
}
void UnitCell::transformB(Eigen::Vector3d& vect) const
{
	vect=_B*vect;
}
void UnitCell::transformT(Eigen::Vector3d& vect) const
{
	vect=_T*vect;
}
Eigen::Vector3d UnitCell::convertB(const Eigen::Vector3d& vect)
{
	return _B*vect;
}
Eigen::Vector3d UnitCell::convertT(const Eigen::Vector3d& vect)
{
	return _T*vect;
}
Eigen::Vector3d UnitCell::convertA(const Eigen::Vector3d& vect)
{
	return _A*vect;
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

UnitCell UnitCell::transformLattice(const Eigen::Matrix3d& P)
{
	UnitCell newcell=*this;
	newcell._A=newcell._A*P;
	const Eigen::Vector3d& av=newcell._A.col(0);
	const Eigen::Vector3d& bv=newcell._A.col(1);
	const Eigen::Vector3d& cv=newcell._A.col(2);
	double a=av.norm();
	double b=bv.norm();
	double c=cv.norm();
	double alpha=acos(bv.dot(cv)/b/c);
	double beta=acos(av.dot(cv)/a/c);
	double gamma=acos(av.dot(bv)/a/b);
	newcell.setCell(a,b,c,alpha,beta,gamma);
	return newcell;
}

void UnitCell::transformLatticeInPlace(const Eigen::Matrix3d& P)
{

	_A=_A*P;
	const Eigen::Vector3d& av=_A.col(0);
	const Eigen::Vector3d& bv=_A.col(1);
	const Eigen::Vector3d& cv=_A.col(2);
	double a=av.norm();
	double b=bv.norm();
	double c=cv.norm();
	double alpha=acos(bv.dot(cv)/b/c);
	double beta=acos(av.dot(cv)/a/c);
	double gamma=acos(av.dot(bv)/a/b);
	setCell(a,b,c,alpha,beta,gamma);
}

}
}
