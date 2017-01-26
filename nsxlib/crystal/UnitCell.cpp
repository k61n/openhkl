#include <cmath>
#include <iomanip>
#include <stdexcept>

#include "UnitCell.h"
#include "Units.h"
#include "gcd.h"

namespace SX
{

namespace Crystal
{

UnitCell::UnitCell()
: SX::Geometry::Basis(),
  _material(),
  _centring(LatticeCentring::P),
  _bravaisType(BravaisType::Triclinic),
  _Z(1),
  _group("P 1"),
  _name("uc"),
  _hklTolerance(0.2)
{
}


UnitCell::UnitCell(double a, double b, double c, double alpha, double beta, double gamma, LatticeCentring centring,BravaisType bravais,std::shared_ptr<SX::Geometry::Basis> reference)
: _material(),
  _centring(centring),
  _bravaisType(bravais),
  _Z(1),
  _group("P 1"),
  _name("uc"),
  _hklTolerance(0.2)
{
    // b-matrix as defined by Busing-Levy paper
    // b1, b2*cos(beta3),  b3*cos(beta2)
    // 0 , b2*sin(beta3), -b3*sin(beta2)*cos(alpha1)
    // 0,  0            ,  1/a3
    // We use the contravariant basis in reciprocal space, so transpose this.

    double ca=cos(alpha), cb=cos(beta), cc=cos(gamma);
    double sa=sin(alpha), sb=sin(beta), sc=sin(gamma);
    double volume=a*b*c*sqrt(1.0-ca*ca-cb*cb-cc*cc+2.0*ca*cb*cc);

    double as=b*c*sa/volume;
    double bs=a*c*sb/volume;
    double cs=a*b*sc/volume;
    double alphas=(cb*cc-ca)/(sb*sc);
    double betas=(ca*cc-cb)/(sa*sc);
    double gammas=(ca*cb-cc)/(sa*sb);

    alphas=acos(alphas);
    betas=acos(betas);
    gammas=acos(gammas);

    _B << as,0,0,
          bs*cos(gammas),bs*sin(gammas),0,
          cs*cos(betas),-cs*sin(betas)*ca,1.0/c;

    _A=_B.inverse();
    SX::Geometry::Basis::_reference=reference;
}

UnitCell::UnitCell(const UnitCell& other)
: SX::Geometry::Basis(other),
  _material(other._material),
  _centring(other._centring),
  _bravaisType(other._bravaisType),
  _Z(other._Z),
  _group(other._group),
  _name(other._name),
  _hklTolerance(other._hklTolerance)
{
}

UnitCell& UnitCell::operator=(const UnitCell& other)
{
    if (this!=&other) {
        _A=other._A;
        _B=other._B;
        _reference=other._reference;
        _hasSigmas=other._hasSigmas;
        if (_hasSigmas) {
            _Acov = other._Acov;
            _Bcov = other._Bcov;
        }
        _material=other._material;
        _centring=other._centring;
        _bravaisType=other._bravaisType;
        _Z=other._Z;
        _group=other._group;
        _name = other._name;
        _hklTolerance = other._hklTolerance;
    }
    return *this;
}

UnitCell::UnitCell(const Eigen::Vector3d& v1,const Eigen::Vector3d& v2,const Eigen::Vector3d& v3, LatticeCentring centring,BravaisType bravais,std::shared_ptr<SX::Geometry::Basis> reference)
: SX::Geometry::Basis(v1,v2,v3,reference),
  _material(),
  _centring(centring),
  _bravaisType(bravais),
  _Z(1),
  _group("P 1"),
  _name("uc"),
  _hklTolerance(0.2)
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
    _B = _A.inverse();
}

void UnitCell::setABCDEF(double A, double B, double C, double D, double E, double F)
{
    double a, b, c, alpha, beta, gamma;
    a = std::sqrt(A);
    b = std::sqrt(B);
    c = std::sqrt(C);
    alpha = std::acos(D / b / c);
    beta = std::acos(E / a / c);
    gamma = std::acos(F / a / b);

    setParams(a, b, c, alpha, beta, gamma);
}

Eigen::Vector3d UnitCell::getAVector() const
{
    return _A.col(0);
}
Eigen::Vector3d UnitCell::getBVector() const
{
    return _A.col(1);
}
Eigen::Vector3d UnitCell::getCVector() const
{
    return _A.col(2);
}
Eigen::Vector3d UnitCell::getReciprocalAVector() const
{
    return _B.row(0);
}
Eigen::Vector3d UnitCell::getReciprocalBVector() const
{
    return _B.row(1);
}
Eigen::Vector3d UnitCell::getReciprocalCVector() const
{
    return _B.row(2);
}

UnitCell UnitCell::fromDirectVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c, LatticeCentring centring, BravaisType bravais,const std::shared_ptr<SX::Geometry::Basis>& reference)
{
    if (coplanar(a,b,c)) {
        throw std::runtime_error("Class UnitCell: the direct vectors are coplanar.");
    }
    return UnitCell(a,b,c,centring,bravais,reference);
}

//! Build a basis from a set of three reciprocal vectors.
UnitCell UnitCell::fromReciprocalVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c,LatticeCentring centring, BravaisType bravais,const std::shared_ptr<SX::Geometry::Basis>& reference)
{
    if (coplanar(a,b,c)) {
        throw std::runtime_error("Class UnitCell: the reciprocal vectors are coplanar.");
    }
    double rVolume = std::abs(a.dot(b.cross(c)));
    Vector3d av = b.cross(c)/rVolume;
    Vector3d bv = c.cross(a)/rVolume;
    Vector3d cv = a.cross(b)/rVolume;

    return UnitCell(av,bv,cv,centring,bravais,reference);
}

void UnitCell::copyMatrices(const UnitCell& other)
{
    _A = other._A;
    _B = other._B;
    _Acov = other._Acov;
    _Bcov = other._Bcov;
    return;
}

void UnitCell::setLatticeVectors(const Vector3d& a, const Vector3d& b, const Vector3d& c)
{
    if (coplanar(a,b,c)) {
        throw std::runtime_error("Class UnitCell: the direct vectors are coplanar.");
    }
    _A.col(0)=a;
    _A.col(1)=b;
    _A.col(2)=c;
    _B=_A.inverse();
}

UnitCell::~UnitCell()
{
}

void UnitCell::setLatticeCentring(LatticeCentring centring)
{
    _centring = centring;
}

void UnitCell::setBravaisType(BravaisType bravais)
{
    _bravaisType = bravais;
}
std::string UnitCell::getBravaisTypeSymbol() const
{
    std::ostringstream os;
    os << static_cast<char>(_bravaisType) << static_cast<char>(_centring);
    return os.str();
}

void UnitCell::setBU(const Vector3d& hkl1, const Vector3d& hkl2, const Vector3d& q1, const Vector3d& q2)
{
    // Get Q1 and Q2 in the diffractometer basis
    Eigen::Vector3d q3u=q1.cross(q2);
    q3u.normalize();
    Vector3d q1u=q1.normalized();
    Vector3d q2u=q3u.cross(q1u);

    //
    Eigen::Vector3d q1prime=this->toReciprocalStandard(hkl1);
    q1prime.normalize();

    Eigen::Vector3d q2prime=this->toReciprocalStandard(hkl2);
    //
    Eigen::Vector3d q3prime=q1prime.cross(q2prime);
    q3prime.normalize();
    q2prime=q3prime.cross(q1prime);

    //
    Eigen::Matrix3d m1,m2,U;
    m1.col(0)=q1u;
    m1.col(1)=q2u;
    m1.col(2)=q3u;

    m2.col(0)=q1prime;
    m2.col(1)=q2prime;
    m2.col(2)=q3prime;

    U=m2*m1.inverse();

    _B=_B*U;
    _A=_B.inverse();
}

void UnitCell::setBU(const Eigen::Matrix3d& BU)
{
    _B = BU;
    _A = _B.inverse();
}

double UnitCell::getD(int h, int k, int l)
{
    const Eigen::Vector3d b1(getReciprocalAVector());
    const Eigen::Vector3d b2(getReciprocalBVector());
    const Eigen::Vector3d b3(getReciprocalCVector());
    const Eigen::Vector3d q = h*b1 + k*b2 + l*b3;
    return SX::Utils::gcd(h, k, l) / q.norm();
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
    B <<  b1,b2*cos(beta3),b3*cos(beta2),
    0	  , b2*sin(beta3), -b3*sin(beta2)*cos(alpha1),
    0	  , 0 , 1/c;

    return B;
}

Eigen::Matrix3d UnitCell::getBusingLevyU() const
{
    return (_B.transpose()*getBusingLevyB().inverse());
}

void UnitCell::printSelf(std::ostream& os) const
{
    os << "Space group:" << std::endl;
    os << _group << std::endl;
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
    os << "UB matrix (in Busing Levy convention):" << std::endl;
    os << _B.transpose() << std::endl;
    //
    if (_material)
    {
        os << *(_material) << std::endl;
        os << "Molar mass: "<< _material->getMolarMass()/SX::Units::g << "g.mol-1 \n";
        os << "Density:" << _material->getMassDensity()/SX::Units::g_per_cm3 << "g.cm-3\n";
        os << "Linear absorption coef: " << _material->getMuAbsorption()*SX::Units::cm << "cm-1 @ 1.798 AA \n";
        os << "Linear incoherent coef: " << _material->getMuScattering()*SX::Units::cm << "cm-1";
    }


}

std::ostream& operator<<(std::ostream& os,const UnitCell& uc)
{
    uc.printSelf(os);
    return os;
}

std::vector<Eigen::Vector3d> UnitCell::generateReflectionsInSphere(double dstarmax) const
{
    std::vector<Eigen::Vector3d> hkls;

    // Get the bounding cube in h,k,l
    int hmax=std::ceil(dstarmax/_B.row(0).norm());
    int kmax=std::ceil(dstarmax/_B.row(1).norm());
    int lmax=std::ceil(dstarmax/_B.row(2).norm());

    std::cout<<"hmax --> "<<hmax<<" "<<kmax<<" "<<lmax<<std::endl;

    int deltah=2*hmax+1;
    int deltak=2*kmax+1;
    int deltal=2*lmax+1;

    hkls.reserve(deltah*deltak*deltal);

    SX::Crystal::SpaceGroup group(getSpaceGroup());

    // Iterate over the cuve and insert element in the map if dstar is not exceeded
    for (int h=-hmax;h<=hmax;++h) {
        for (int k=-kmax;k<=kmax;++k) {
            for (int l=-lmax;l<=lmax;++l) {
                // jmf added 29.08.2016
                // skip those HKL which are forbidden by the space group??
                if (group.isExtinct(h, k, l)) {
                    continue;
                }

                Eigen::Vector3d hkl(h,k,l);
                auto q=toReciprocalStandard(hkl);
                double norm=q.norm();
                if (norm < dstarmax) {
                    hkls.push_back(hkl);
                }
            }
        }
    }
    return hkls;
}

std::vector<Eigen::Vector3d> UnitCell::generateReflectionsInShell(double dmin, double dmax, double wavelength) const
{
    const Eigen::Vector3d b1(getReciprocalAVector());
    const Eigen::Vector3d b2(getReciprocalBVector());
    const Eigen::Vector3d b3(getReciprocalCVector());
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver;
    const Eigen::Matrix3d BTB(getReciprocalMetricTensor());
    std::vector<Eigen::Vector3d> hkls;

    eigen_solver.compute(BTB);
    double b_min = std::sqrt(eigen_solver.eigenvalues().minCoeff());
    const int hkl_max = std::ceil(2.0 / (wavelength * b_min));
    const int num_hkl = 2*hkl_max+1;
    hkls.reserve(num_hkl*num_hkl*num_hkl);

    SX::Crystal::SpaceGroup group(getSpaceGroup());

    for (int h = -hkl_max; h <= hkl_max; ++h) {
        for (int k = -hkl_max; k <= hkl_max; ++k) {
            for (int l = -hkl_max; l <= hkl_max; ++l) {

                Eigen::Vector3d q = h*b1 + k*b2 + l*b3;

//                double gcd = (double)SX::Utils::gcd(h, k, l);
//                const double d = gcd / q.norm();
                const double d = 1.0 / q.norm();

                const double sin_theta = wavelength / (2.0 * d);

                // unphysical, so skip
                if (sin_theta > 1.0 && sin_theta < 0.0) {
                    continue;
                }

                // scattering angle too large
                if ( d < dmin) {
                    continue;
                }

                // scattering angle too small
                if ( d > dmax) {
                    continue;
                }

                // skip those HKL which are forbidden by the space group
                if (group.isExtinct(h, k, l)) {
                    continue;
                }

                hkls.emplace_back(Eigen::Vector3i(h, k, l).cast<double>());
            }
        }
    }
    return hkls;
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

    if (value<-1) {
        return M_PI;
    }
    if (value>1) {
        return 0;
    }
    return acos(q1.dot(q2)/q1.norm()/q2.norm());
}

bool UnitCell::isEquivalent(double h1, double k1, double l1, double h2, double k2, double l2) const
{
    return _group.isEquivalent(h1, k1, l1, h2, k2, l2);
}

bool UnitCell::isFriedelEquivalent(double h1, double k1, double l1, double h2, double k2, double l2) const
{
    return _group.isFriedelEquivalent(h1, k1, l1, h2, k2, l2);
}

unsigned int UnitCell::getZ() const
{
    return _Z;
}

void UnitCell::setZ(unsigned int Z)
{
    _Z = Z;
}

Chemistry::sptrMaterial UnitCell::getMaterial() const
{
    return _material;
}

void UnitCell::setMaterial(const Chemistry::sptrMaterial& material)
{
    _material = material;
}

void UnitCell::setSpaceGroup(const std::string& symbol)
{
    _group = SpaceGroup(symbol);
}

std::string UnitCell::getSpaceGroup() const
{
    return _group.getSymbol();
}

void UnitCell::setName(const std::string& name)
{
	if (name.empty())
		return;
	_name = name;
}

const std::string& UnitCell::getName() const
{
	return _name;
}

void UnitCell::setHKLTolerance(double tolerance)
{
	if (tolerance <= 0.0 || tolerance >= 1.0)
		throw std::runtime_error("Class UnitCell: invalid integer HKL tolerance.");
	_hklTolerance = tolerance;
}

double UnitCell::getHKLTolerance() const
{
	return _hklTolerance;
}

} // end namespace Chemistry
} // end namespace SX
