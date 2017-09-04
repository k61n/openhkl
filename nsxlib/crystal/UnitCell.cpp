#include <iostream>
#include <cmath>
#include <iomanip>
#include <stdexcept>

#include "../chemistry/Material.h"
#include "../crystal/UnitCell.h"
#include "../crystal/NiggliReduction.h"
#include "../crystal/GruberReduction.h"
#include "../mathematics/GCD.h"
#include "../mathematics/Minimizer.h"
#include "../utils/Units.h"

namespace nsx {

CellCharacter::CellCharacter():
    A(0.0), B(0.0), C(0.0), D(0.0), E(0.0), F(0.0),
    a(0.0), b(0.0), c(0.0), alpha(0.0), beta(0.0), gamma(0.0)
{
}

CellCharacter::CellCharacter(const Eigen::Matrix3d& g): 
    CellCharacter(g(0,0), g(1,1), g(2,2), g(1,2), g(0,2), g(0,1))
{
}

CellCharacter::CellCharacter(double A_, double B_, double C_, double D_, double E_, double F_)
{
    A = A_;
    B = B_;
    C = C_;
    D = D_;
    E = E_;
    F = F_;

    a = std::sqrt(A);
    b = std::sqrt(B);
    c = std::sqrt(C);

    alpha = std::acos(D / b / c);
    beta  = std::acos(E / a / c);
    gamma = std::acos(F / a / b);
}

UnitCell::UnitCell(const Eigen::Matrix3d& b, bool reciprocal): UnitCell()
{
    _A = reciprocal ? b.inverse() : b;
    _B = reciprocal ? b : b.inverse();
}

UnitCell::UnitCell():
    _material(),
    _centring(LatticeCentring::P),
    _bravaisType(BravaisType::Triclinic),
    _Z(1),
    _group("P 1"),
    _name("uc"),
    _hklTolerance(0.2),
    _niggli(),
    _A(Eigen::Matrix3d::Identity()),
    _B(Eigen::Matrix3d::Identity()),
    _Acov(),
    _Bcov(), 
    _NP(Eigen::Matrix3d::Identity())
{
}

UnitCell::UnitCell(double a, double b, double c, double alpha, double beta, double gamma): UnitCell()
{
    setParams(a,b,c,alpha,beta,gamma);
}

UnitCell::UnitCell(const UnitCell& other)
: UnitCell()
{
    *this = other;
}

UnitCell& UnitCell::operator=(const UnitCell& other)
{
    if (this == &other) {
        return *this;
    }
    _A = other._A;
    _B = other._B;
    _Acov = other._Acov;
    _Bcov = other._Bcov;
    _material = other._material;
    _centring = other._centring;
    _bravaisType = other._bravaisType;
    _Z = other._Z;
    _group=other._group;
    _name = other._name;
    _hklTolerance = other._hklTolerance;
    _niggli = other._niggli;
    _NP = other._NP;
    return *this;
}

void UnitCell::setParams(double a, double b, double c, double alpha, double beta, double gamma)
{
    const double cos_alpha = std::cos(alpha);
    const double cos_beta = std::cos(beta);
    const double cos_gamma = std::cos(gamma);
    const double sin_gamma = std::sin(gamma);

    const double a1 = a;
    const double b1 = b*cos_gamma;
    const double b2 = b*sin_gamma;

    const double c1 = c*cos_beta;
    const double c2 = (c*cos_alpha - c1*cos_gamma) / sin_gamma;
    const double c3 = std::sqrt(c*c - c1*c1 - c2*c2);

    _A << 
        a1, b1, c1,
        0, b2, c2,
        0, 0, c3;

    _B = _A.inverse();

    // reset covariance
    _Acov *= 0;
    _Bcov *= 0;
}

void UnitCell::setABCDEF(double A, double B, double C, double D, double E, double F)
{
    // make sure the parameters are in the valid range
    A = std::fabs(A);
    B = std::fabs(B);
    C = std::fabs(C);

    double a, b, c, alpha, beta, gamma;
    a = std::sqrt(A);
    b = std::sqrt(B);
    c = std::sqrt(C);

    // more checking
    if (std::fabs(D) > b*c) {
        D = (std::signbit(D) ? -1 : 1) * b * c;
    }
    if (std::fabs(E) > a*c) {
        E = (std::signbit(E) ? -1 : 1) * a * c;
    }
    if (std::fabs(F) > a*b) {
        F = (std::signbit(F) ? -1 : 1) * a * b;
    }

    alpha = std::acos(D / b / c);
    beta = std::acos(E / a / c);
    gamma = std::acos(F / a / b);

    setParams(a, b, c, alpha, beta, gamma);
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

double UnitCell::getD(int h, int k, int l)
{
    auto q = fromIndex(Eigen::RowVector3d(h,k,l));
    return gcd(h, k, l) / q.norm();
}


void UnitCell::printSelf(std::ostream& os) const
{
    auto c = character();
    auto rc = reciprocalCharacter();

    os << "Space group:" << std::endl;
    os << _group << std::endl;
    os << "Direct Lattice:\n";
    os << std::fixed << std::setw(10) << std::setprecision(5) << c.a;
    os << std::fixed << std::setw(10) << std::setprecision(5) << c.b;
    os << std::fixed << std::setw(10) << std::setprecision(5) << c.c;
    os << std::fixed << std::setw(10) << std::setprecision(5) << c.alpha/deg;
    os << std::fixed << std::setw(10) << std::setprecision(5) << c.beta/deg;
    os << std::fixed << std::setw(10) << std::setprecision(5) << c.gamma/deg << std::endl;
    os << "Reciprocal Lattice:\n";
    os << std::fixed << std::setw(10) << std::setprecision(5) << rc.a;
    os << std::fixed << std::setw(10) << std::setprecision(5) << rc.b;
    os << std::fixed << std::setw(10) << std::setprecision(5) << rc.c;
    os << std::fixed << std::setw(10) << std::setprecision(5) << rc.alpha/deg;
    os << std::fixed << std::setw(10) << std::setprecision(5) << rc.beta/deg;
    os << std::fixed << std::setw(10) << std::setprecision(5) << rc.gamma/deg << std::endl;
    os << "Reciprocal basis (row vectors):" << std::endl;
    os << _B << std::endl;
    //
    if (_material) {
        os << *(_material) << std::endl;
        os << "Molar mass: "<< _material->molarMass()/g << "g.mol-1 \n";
        os << "Density:" << _material->massDensity()/g_per_cm3 << "g.cm-3\n";
        os << "Linear absorption coef: " << _material->muAbsorption()*cm << "cm-1 @ 1.798 AA \n";
        os << "Linear incoherent coef: " << _material->muIncoherent()*cm << "cm-1";
    }
}

std::ostream& operator<<(std::ostream& os,const UnitCell& uc)
{
    uc.printSelf(os);
    return os;
}

// todo: check this again
std::vector<Eigen::RowVector3d> UnitCell::generateReflectionsInShell(double dmin, double dmax, double wavelength) const
{
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver;
    std::vector<Eigen::RowVector3d> hkls;
    eigen_solver.compute(reciprocalMetric());
    double b_min = std::sqrt(eigen_solver.eigenvalues().minCoeff());
    const int hkl_max = std::ceil(2.0 / (wavelength * b_min)); // maximum allowed by Bragg law
    const int num_hkl = 2*hkl_max+1;
    hkls.reserve(num_hkl*num_hkl*num_hkl);

    SpaceGroup group(getSpaceGroup());

    for (int h = -hkl_max; h <= hkl_max; ++h) {
        for (int k = -hkl_max; k <= hkl_max; ++k) {
            for (int l = -hkl_max; l <= hkl_max; ++l) {
                Eigen::RowVector3d hkl(h,k,l);
                Eigen::RowVector3d q = hkl*_B;
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
                hkls.emplace_back(hkl);
            }
        }
    }
    return hkls;
}


double UnitCell::getAngle(const Eigen::RowVector3d& hkl1, const Eigen::RowVector3d& hkl2) const
{
    auto q1=hkl1*_B;
    auto q2=hkl2*_B;    
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

sptrMaterial UnitCell::getMaterial() const
{
    return _material;
}

void UnitCell::setMaterial(const sptrMaterial& material)
{
    _material = material;
}

void UnitCell::setSpaceGroup(const std::string& symbol)
{
    _group = SpaceGroup(symbol);
}

std::string UnitCell::getSpaceGroup() const
{
    return _group.symbol();
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

void UnitCell::setNiggli(const NiggliCharacter& niggli)
{
    _niggli = niggli;
}

UnitCell UnitCell::applyNiggliConstraints(double weight) const
{
    // no constraints for these cases to early return
    if (_niggli.number == 31 || _niggli.number == 44) {
        return *this;
    }

    Eigen::MatrixXd C = _niggli.C;
    Eigen::Matrix3d B0 = _NP*_B;
    Eigen::Matrix3d A0 = _A*_NP.inverse();
    
    const double a = std::pow(std::fabs(A0.determinant()), 1.0/3.0);
    const double b = std::pow(std::fabs(B0.determinant()), 1.0/3.0);

    // number of constraints on the 6 lattice parameters
    const int nconstraints = C.rows();
    // free parameters in character + euler angles
    const int nparams = 3 + (6-nconstraints); 

    // used for the space of constraints
    Eigen::FullPivLU<Eigen::MatrixXd> lu(C);
    Eigen::MatrixXd kernel = lu.kernel();

    // get rotation matrix from three parameters via quaternion
    auto rotation = [](const Eigen::Vector3d& x) -> Eigen::Matrix3d {
        const double d = 1.0 / std::sqrt(1 + x.squaredNorm());
        // quaternion representing rotation
        Eigen::Quaterniond q(d, d*x(0), d*x(1), d*x(2));
        // rotation matrix
        return q.toRotationMatrix();
    };

    // get initial orientation matrix
    Eigen::Matrix3d U0 = UnitCell(niggliBasis()).orientation();

    auto new_uc = [=](const Eigen::VectorXd& x) {
        // lattice character
        Eigen::VectorXd ch(6);
        ch.setZero();        
        // parameters defining lattice chatacer
        for (auto i = 3; i < nparams; ++i) {
            ch += x(i)*kernel.col(i-3);
        }
        // get new unit cell
        UnitCell uc;
        // set lattice parameters
        uc.setABCDEF(ch(0), ch(1), ch(2), ch(3), ch(4), ch(5));
        // rotation matrix
        Eigen::Matrix3d U1 = rotation({x(0), x(1), x(2)});
        // rotate unit cell
        uc.setBasis(U1*U0*uc._A*_NP);
        return uc;
    };

    // residuals used for least-squares fitting
    auto functor = [=](const Eigen::VectorXd& x, Eigen::VectorXd& residuals) -> int {
        UnitCell uc = new_uc(x);
        Eigen::Matrix3d A = uc.basis();
        Eigen::Matrix3d B = uc.reciprocalBasis();

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                residuals(2*(3*i+j)+0) = (B(i,j) - _B(i, j)) / b;
                residuals(2*(3*i+j)+1) = (A(i,j) - _A(i, j)) / a;
            }
        }
 
        return 0;
    };

    nsx::Minimizer min;

    // starting parameters
    Eigen::VectorXd p(nparams);
    p.setZero();

    // lattice character
    Eigen::Matrix3d G = A0.transpose()*A0;
    Eigen::VectorXd ch(6);
    ch << G(0,0), G(1,1), G(2,2), G(1,2), G(0,2), G(0,1);
    
    // get starting values
    auto y = (kernel.transpose()*kernel).inverse()*kernel.transpose()*ch;
    for (auto i = 0; i < nparams; ++i) {
        p(i) = i < 3 ? 0.0 : y(i-3);
    }

    min.initialize(nparams, 9+9);
    min.setParams(p);
    min.set_f(functor);

    min.setxTol(1e-6);
    min.setfTol(1e-6);
    min.setgTol(1e-6);

    if (!min.fit(100)) {
        throw std::runtime_error("ERROR: failed to apply Niggli constraints!");
    }
    return new_uc(min.params());
}

Eigen::RowVector3d UnitCell::index(const Eigen::RowVector3d& q) const
{
    return q*_A;
}

Eigen::RowVector3d UnitCell::fromIndex(const Eigen::RowVector3d& hkl) const
{
    return hkl*_B;
}
    
CellCharacter UnitCell::character() const
{
    return CellCharacter(metric());
}

//! we calculate errors in the unit cell parameters using a simple propagation of error.
//! For a function f(x) of a random variable x, with variance-covariance matrix C, we perform
//! the first order approximation
//!
//! f(x) ~ f(mu) + f'(mu)(x-mu)
//!
//! Using this approximation, we obtain the approxmation
//!
//! sigma^2(f) ~ f'(mu).dot(C*f'(mu))
//!
//! The non-trivial part of this function is to calculate the derivatives f' of the lattice
//! parameters as functions of the components of the unit cell basis vectors.
CellCharacter UnitCell::characterSigmas() const
{   
    CellCharacter sigma;
    double sigma_abc[6];
    double sigma_ABC[6];

    // flatten 3x3 matrix into a 9-vector
    auto idx = [](int i, int j) { return 3*i+j; };

    for (int col = 0; col < 3; ++col) {
        Eigen::Matrix<double, 9, 1> J;
        J.setZero();
        auto col1 = (col+1)%3;
        auto col2 = (col+2)%3;
        const auto& a = _A.col(col);
        const auto& b = _A.col(col1);
        const auto& c = _A.col(col2);
        const auto& cov = _Acov;

        // compute Jacobian for norm-squared of the column
        for (int row = 0; row < 3; ++row) {
            auto rc = idx(row,col);
            J(rc) = 2.0*a(row);
        }
        const double sigma_sq = std::sqrt(J.dot(cov*J));
        // error in squared-norm
        sigma_ABC[col] = sigma_sq;
        // error in norm
        sigma_abc[col] = 0.5 * sigma_sq / a.norm();

        // compute the Jacobian for D = b.c where b and c are the other two columns
        J.setZero();
        for (int row = 0; row < 3; ++row) {
            auto rc = idx(row,col1);
            J(rc) = c(row);
            rc = idx(row,col2);
            J(rc) = b(row);
        }
        sigma_ABC[3+col] = std::sqrt(J.dot(cov*J));

        // compute the Jacobian for alpha = acos(b.c/|b||c|) where b and c are the other two columns
        J.setZero();
        const double nb = b.norm();
        const double nc = c.norm();
        const double bc = b.dot(c);
        const double factor = -1.0 / std::sqrt(nb*nb*nc*nc - bc*bc);
        for (int row = 0; row < 3; ++row) {
            auto rc = idx(row,col1);
            J(rc) = factor * (c(row)  + bc*b(row) / nb / nb);
            rc = idx(row,col2);
            J(rc) = factor * (b(row)  + bc*c(row) / nc / nc);
        }
        sigma_abc[3+col] = std::sqrt(J.dot(cov*J));
    }

    sigma.A = sigma_ABC[0];
    sigma.B = sigma_ABC[1];
    sigma.C = sigma_ABC[2];
    sigma.D = sigma_ABC[3];
    sigma.E = sigma_ABC[4];
    sigma.F = sigma_ABC[5];

    sigma.a = sigma_abc[0];
    sigma.b = sigma_abc[1];
    sigma.c = sigma_abc[2];
    sigma.alpha = sigma_abc[3];
    sigma.beta = sigma_abc[4];
    sigma.gamma = sigma_abc[5];

    return sigma;
}

CellCharacter UnitCell::reciprocalCharacter() const
{
    return CellCharacter(reciprocalMetric());
}

double UnitCell::volume() const
{
    return std::fabs(_A.determinant());
}

Eigen::Matrix3d UnitCell::metric() const
{
    return _A.transpose()*_A;
}

Eigen::Matrix3d UnitCell::reciprocalMetric() const
{
    return _B*_B.transpose();
}

const Eigen::Matrix3d& UnitCell::basis() const
{
    return _A;
}

const Eigen::Matrix3d& UnitCell::reciprocalBasis() const
{
    return _B;
}

void UnitCell::setReciprocalBasis(const Eigen::Matrix3d& B)
{
    _B = B;
    _A = B.inverse();
    // note: we lose covariance invormation!
    _Acov *= 0;
    _Bcov *= 0;
}

void UnitCell::transform(const Eigen::Matrix3d& P)
{
    _A = _A*P;
    _B = _A.inverse();
    _Acov = transformCovariance(Eigen::Matrix3d::Identity(), P, _Acov);
    _Bcov = transformCovariance(_A, _A, _Acov);
}

int UnitCell::reduce(bool niggli_only, double niggliTolerance, double gruberTolerance)
{
    // first find Niggli-reduced cell (required for Gruber)
    NiggliReduction niggli(metric(), niggliTolerance);
    Eigen::Matrix3d newg, P;
    niggli.reduce(newg, P);
    transform(P);

    // use GruberReduction::reduce to get Bravais type
    GruberReduction gruber(metric(), gruberTolerance);
    LatticeCentring c;
    BravaisType b;

    try {
        gruber.reduce(P,c,b);
        setLatticeCentring(c);
        setBravaisType(b);
        _niggli = gruber.classify();
    }
    catch(std::exception& e) {
        //qDebug() << "Gruber reduction error:" << e.what();
        //continue;
    }

    if (niggli_only == false) {
        transform(P);
        _NP = P;
    } else {
        _NP = Eigen::Matrix3d::Identity();
    }
    return _niggli.number;
}

Eigen::Matrix3d UnitCell::busingLevyB() const
{
    return orientation().transpose() * _B;
}

Eigen::Matrix3d UnitCell::busingLevyU() const
{
    return orientation();
}

void UnitCell::setBasis(const Eigen::Matrix3d& b)
{
    _A = b;
    _B = b.inverse();
}

// For uncorrelated errors, given a matrix A with inverse A-1,
// then the uncertainty squared [sigmaA-1]^2_{alpha,beta}=[A-1]^2_{alpha,i}.[sigmaA]^2_{i,j}.[A-1]^2_{j,beta}
// Here, use the fact that A^-1=B, or B^-1=A when converting from direct to reciprocal or reciprocal to direct
// Explained in M. Lefebvre, R.K. Keeler, R. Sobie, J. White, Propagation of errors for matrix inversion,
// Nuclear Instruments and Methods in Physics Research Section A: Accelerators, Spectrometers, Detectors and Associated Equipment, Volume 451, Issue 2, 1 September 2000, Pages 520-528
Eigen::Matrix<double, 9, 9> UnitCell::transformCovariance(const Eigen::Matrix3d& M, const Eigen::Matrix3d& N, const Eigen::Matrix<double, 9, 9>& C)
{
    Eigen::Matrix<double, 9, 9> R;

    auto unpack = [](int ij, int& i, int& j) -> void
    {
        j = ij%3;
        i = ij/3;
    };

    for (int ij = 0; ij < 9; ++ij) {
        int i, j;
        unpack(ij, i, j);
        for (int kl = 0; kl < 9; ++kl) {
            int k, l;
            unpack(kl, k, l);
            R(ij, kl) = 0.0;
            for (int ab = 0; ab < 9; ++ab) {
                int a, b;
                unpack(ab, a, b);
                for (int cd = 0; cd < 9; ++cd) {
                    int c, d;
                    unpack(cd, c, d);
                    const double C_ijab = M(i, a) * N(b, j);
                    const double C_klcd = M(k, c) * N(d, l);
                    R(ij, kl) += C_ijab * C(ab, cd) * C_klcd;
                }
            }
        }
    }
    return R;
}

const Eigen::Matrix<double, 9, 9>& UnitCell::covariance() const
{
    return _Acov;
}

const Eigen::Matrix<double, 9, 9>& UnitCell::reciprocalCovariance() const
{
    return _Bcov;
}

void UnitCell::setCovariance(const Eigen::MatrixXd& Acov)
{
    _Acov = Acov;
    _Bcov = transformCovariance(_B, _B, _Acov);
}

void UnitCell::setReciprocalCovariance(const Eigen::MatrixXd& Bcov)
{
    _Bcov = Bcov;
    _Acov = transformCovariance(_A, _A, _Bcov);
}

Eigen::Matrix3d UnitCell::covariance(int col) const
{
    Eigen::Matrix3d cov;
    auto index = [](int i, int j) { return 3*i+j;};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cov(i, j) = _Acov(index(i, col), index(j, col));
        }
    }
    return cov;
}

Eigen::Matrix3d UnitCell::reciprocalCovariance(int row) const
{
    Eigen::Matrix3d cov;
    auto index = [](int i, int j) { return 3*i+j;};
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            cov(i, j) = _Acov(index(row, i), index(row, j));
        }
    }
    return cov;
}

const NiggliCharacter& UnitCell::niggliCharacter() const
{
    return _niggli;
}

Eigen::Matrix3d UnitCell::niggliBasis() const
{
    return _A*_NP.inverse();
}

Eigen::Matrix3d UnitCell::reciprocalNiggliBasis() const
{
    return _NP*_B;
}

bool UnitCell::equivalent(const UnitCell& other, double tolerance) const
{
    const Eigen::Matrix3d B1 = reciprocalNiggliBasis();
    const Eigen::Matrix3d B2 = other.reciprocalNiggliBasis();

    Eigen::Matrix3d T = B2 * B1.inverse();
    Eigen::Matrix3d S = B1 * B2.inverse();

    // round to integer coefficients
    for (auto i = 0; i < 3; ++i) {
        for (auto j = 0; j < 3; ++j) {
            T(i,j) = std::round(T(i,j));
            S(i,j) = std::round(S(i,j));
        }
    }

    // check whether T and S are inverse
    if ((T*S - Eigen::Matrix3d::Identity()).norm() > tolerance) {
        return false;
    }

    // check that B2 = T B1
    if ((B2 - T*B1).norm() > tolerance * B2.norm()) {
        return false;
    }

    // check that B1 = S * B2
    if ((B1 - S*B2).norm() > tolerance * B1.norm()) {
        return false;
    }

    return true;
}


const Eigen::Matrix3d& UnitCell::niggliTransformation() const
{
    return _NP;
}

Eigen::Matrix3d UnitCell::orientation() const
{
    Eigen::Matrix3d Q = _A.householderQr().householderQ();
    Eigen::Matrix3d R = Q.transpose() * _A;

    for (auto i = 0; i < 3; ++i) {
        if (R(i,i) < 0) {
            Q.col(i) *= -1.0;
        }
    }
    return Q;
}

} // end namespace nsx
