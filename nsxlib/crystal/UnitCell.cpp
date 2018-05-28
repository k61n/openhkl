#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "GCD.h"
#include "GruberReduction.h"
#include "Material.h"
#include "MillerIndex.h"
#include "Minimizer.h"
#include "NiggliReduction.h"
#include "ReciprocalVector.h"
#include "UnitCell.h"
#include "Units.h"

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
    _A(Eigen::Matrix3d::Identity()),
    _B(Eigen::Matrix3d::Identity()),
    _NP(Eigen::Matrix3d::Identity()),
    _material(),
    _centring(LatticeCentring::P),
    _bravaisType(BravaisType::Triclinic),
    _Z(1),
    _group("P 1"),
    _name("uc"),
    _indexingTolerance(0.2),
    _niggli()
{
}

UnitCell::UnitCell(double a, double b, double c, double alpha, double beta, double gamma): UnitCell()
{
    setParams(a,b,c,alpha,beta,gamma);
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

void UnitCell::setLatticeCentring(LatticeCentring centring)
{
    _centring = centring;
}

void UnitCell::setBravaisType(BravaisType bravais)
{
    _bravaisType = bravais;
}

std::string UnitCell::bravaisTypeSymbol() const
{
    std::ostringstream os;
    os << static_cast<char>(_bravaisType) << static_cast<char>(_centring);
    return os.str();
}

double UnitCell::d(int h, int k, int l)
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
std::vector<MillerIndex> UnitCell::generateReflectionsInShell(double dmin, double dmax, double wavelength) const
{
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver;
    std::vector<MillerIndex> hkls;
    eigen_solver.compute(reciprocalMetric());
    double b_min = std::sqrt(eigen_solver.eigenvalues().minCoeff());
    const int hkl_max = std::ceil(2.0 / (wavelength * b_min)); // maximum allowed by Bragg law
    const int num_hkl = 2*hkl_max+1;
    hkls.reserve(num_hkl*num_hkl*num_hkl);

    SpaceGroup group(spaceGroup());

    for (int h = -hkl_max; h <= hkl_max; ++h) {
        for (int k = -hkl_max; k <= hkl_max; ++k) {
            for (int l = -hkl_max; l <= hkl_max; ++l) {
                MillerIndex hkl(h, k, l);
                Eigen::RowVector3d q = hkl.rowVector().cast<double>()*_B;
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
                if (group.isExtinct(hkl)) {
                    continue;
                }
                hkls.emplace_back(hkl);
            }
        }
    }
    return hkls;
}


double UnitCell::angle(const Eigen::RowVector3d& hkl1, const Eigen::RowVector3d& hkl2) const
{
    auto q1=hkl1*_B;
    auto q2=hkl2*_B;    
    return std::acos(q1.dot(q2)/q1.norm()/q2.norm());
}

unsigned int UnitCell::z() const
{
    return _Z;
}

void UnitCell::setZ(unsigned int Z)
{
    _Z = Z;
}

sptrMaterial UnitCell::material() const
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

const SpaceGroup& UnitCell::spaceGroup() const
{
    return _group;
}

void UnitCell::setName(const std::string& name)
{
    if (name.empty())
        return;
    _name = name;
}

const std::string& UnitCell::name() const
{
    return _name;
}

void UnitCell::setIndexingTolerance(double tolerance)
{
    if (tolerance <= 0.0 || tolerance >= 1.0)
        throw std::runtime_error("Class UnitCell: invalid integer HKL tolerance.");
    _indexingTolerance = tolerance;
}

double UnitCell::indexingTolerance() const
{
    return _indexingTolerance;
}

void UnitCell::setNiggli(const NiggliCharacter& niggli)
{
    _niggli = niggli;
}

//! This method finds the unit cell closest to the given cell, which also satisfies the 
//! symmetry constraints on the Niggli character (see e.g. tables 9.2.5.1 of volume A of the international tables).
//! Since the constraints are _non-linear_ on the components of the basis matrix, we perform a change of coordinates
//! into a system where the constraints are linear.
//!
//! We parameterize the basis matrix as U1 * U0 * R, where U1 is rotation, U0 is the orientation of the initial guess,
//! and R is upper triangular. The upper triangular matrix R is parameterized by the 6 lattice character components
//! A, B, C, D, E, F on which the constraints are linear. We construct the rotation matrix U1 from a unit quaternion,
//! which is parameterized by 3 real numbers. So the total number of parameters is
//! 3 quaternion components + 6 lattice characters - # constraints.
//!
//! We further note that the factorization U = U1 * U0 is done for numerical stability (since we expect U1 to be small)
//! and that we use quaternions to compute U1 also for numerical stability (and furthermore to guarantee that U1 is
//! unitary, which is a non-linear constraint on its components).
//!
//! Given this parameterization and initial basis matrices A0, B0 = A0^{-1}, we perform a least-squares minimization to
//! find A, B = A^{-1} such that the L2 matrix norms |A-A0|^2 and |B-B0|^2 are minimized.
UnitCell UnitCell::applyNiggliConstraints() const
{
    // no constraints for these cases to early return
    if (_niggli.number == 31 || _niggli.number == 44) {
        return *this;
    }
    
    // geometric mean of side-lengths of unit cell & reciprocal unit cell
    // we use these to scale the residuals in the fitting function below   
    const double a = std::pow(std::fabs(_A.determinant()), 1.0/3.0);
    const double b = std::pow(std::fabs(_B.determinant()), 1.0/3.0);

    // The orientation matrix (in direct space)
    Eigen::Matrix3d U = niggliOrientation();
    // The offsets of the orientation matrix
    Eigen::Vector3d uOffset(0,0,0);
    // The unit cell parameters
    Eigen::VectorXd p = parameters();

    // residuals used for least-squares fitting
    // these are just the differences A-A0 and B-B0 as described above
    auto functor = [&](Eigen::VectorXd& residuals) -> int
    {
        UnitCell uc = this->fromParameters(U, uOffset, p);
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
    FitParameters params;

    for (auto i = 0; i < 3; ++i) {
        params.addParameter(&uOffset(i));
    }

    for (auto i = 0; i < p.size(); ++i) {
        params.addParameter(&p(i));
    }

    min.initialize(params, 9+9);
    min.set_f(functor);

    min.setxTol(1e-6);
    min.setfTol(1e-6);
    min.setgTol(1e-6);

    // note: if the UC already satisfies the constraints, the minimizer will fail with GSL_ENOPROG
    // so we don't check the return value of Minimizer::fit
    min.fit(100);
    nsx::UnitCell new_uc = fromParameters(U, uOffset, p);

    // check if the new UC is close to the old one
    const double delta = (new_uc.reciprocalBasis()-_B).norm() / _B.norm();

    if (delta < 0.1) {
        return new_uc;
    } 
    throw std::runtime_error("ERROR: could not apply symmetry constraints to unit cell");
}

Eigen::RowVector3d UnitCell::index(const ReciprocalVector& q) const
{
    return q.rowVector()*_A;
}

Eigen::RowVector3d UnitCell::fromIndex(const Eigen::RowVector3d& hkl) const
{
    return hkl*_B;
}
    
CellCharacter UnitCell::character() const
{
    return CellCharacter(metric());
}

CellCharacter UnitCell::characterSigmas() const
{   
    return _characterSigmas;
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
}

void UnitCell::transform(const Eigen::Matrix3d& P)
{
    _A = _A*P;
    _B = _A.inverse();
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

void UnitCell::setBasis(const Eigen::Matrix3d& b)
{
    _A = b;
    _B = b.inverse();
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

Eigen::Matrix3d UnitCell::niggliOrientation() const
{
    const Eigen::Matrix3d NA  =_A*_NP.inverse();
    Eigen::Matrix3d Q = NA.householderQr().householderQ();
    Eigen::Matrix3d R = Q.transpose() * NA;

    for (auto i = 0; i < 3; ++i) {
        if (R(i,i) < 0) {
            Q.col(i) *= -1.0;
        }
    }
    return Q;
}

Eigen::VectorXd UnitCell::parameters() const
{
    // note that we use NP to transform to the Niggli cell (in case we are currently a Gruber cell)
    Eigen::Matrix3d A0 = _A*_NP.inverse();
    Eigen::Matrix3d G = A0.transpose()*A0;
    Eigen::VectorXd ch(6);
    ch << G(0,0), G(1,1), G(2,2), G(1,2), G(0,2), G(0,1);

    // no constraints to be applied:
    if (_niggli.number == 31 || _niggli.number == 44) {
        return ch;
    }
    
    // matrix of Niggli character constraints, taken from the table 9.2.5.1
    Eigen::MatrixXd C = _niggli.C;

    // used for the space of constraints
    Eigen::FullPivLU<Eigen::MatrixXd> lu(C);
    Eigen::MatrixXd kernel = lu.kernel();
    
    // get starting values: these are just the lattice character (components of metric tensor)
    return (kernel.transpose()*kernel).inverse()*kernel.transpose()*ch;
}

UnitCell UnitCell::fromParameters(const Eigen::Matrix3d& U0, const Eigen::Vector3d& uOffset, const Eigen::VectorXd& parameters) const
{
    // get new orientation from offsets
    Eigen::Quaterniond q(1.0, uOffset(0), uOffset(1), uOffset(2));
    q.normalize();

    Eigen::Matrix3d U = q.toRotationMatrix()*U0;

    Eigen::MatrixXd kernel;

    // no constraints
    if (_niggli.number == 31 || _niggli.number == 44) {
        kernel.setIdentity(6, 6);
    } else {
        // matrix of Niggli character constraints, taken from the table 9.2.5.1
        Eigen::MatrixXd C = _niggli.C;
        // compute kernel of Niggli constraints
        Eigen::FullPivLU<Eigen::MatrixXd> lu(_niggli.C);
        kernel = lu.kernel();
    }

    const int nparams = kernel.cols();
    assert(nparams == parameters.size());

    // lattice character
    Eigen::VectorXd ch(6);
    ch.setZero();        
    // parameters defining lattice chatacer
    for (auto i = 0; i < nparams; ++i) {
        ch += parameters(i)*kernel.col(i);
    }

    // create new unit cell
    UnitCell uc(*this);
    uc.setABCDEF(ch(0), ch(1), ch(2), ch(3), ch(4), ch(5));
    uc.setBasis(U*uc._A*_NP);

    return uc;
}

//! We calculate errors in the unit cell parameters using a simple propagation of error.
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
void UnitCell::setParameterCovariance(const Eigen::MatrixXd& cov)
{
    const Eigen::VectorXd params = parameters();
    assert(params.size() == cov.rows());
    assert(params.size() == cov.cols());

    // the kernel matrix of the Niggli constraints
    Eigen::MatrixXd kernel;
    
    // no constraints?
    if (_niggli.number == 31 || _niggli.number == 44) {
        kernel.setIdentity(6, 6);
    } else {
        // matrix of Niggli character constraints, taken from the table 9.2.5.1
        Eigen::MatrixXd C = _niggli.C;
        // compute kernel of Niggli constraints
        Eigen::FullPivLU<Eigen::MatrixXd> lu(_niggli.C);
        kernel = lu.kernel();
    }
    
    // covariance matrix of the paramters A, B, C, D, E, F
    Eigen::MatrixXd ABC_cov = kernel.transpose() * cov * kernel;
    
    // lattice character
    CellCharacter ch = character();
    // store character in these arrays to make symbolic calculation easier
    const double ABC[6] = {ch.A, ch.B, ch.C, ch.D, ch.E, ch.F};
    const double abc[6] = {ch.a, ch.b, ch.c, ch.alpha, ch.beta, ch.gamma}; 

    // Jacobian of the transformation (A,B,C,D,E,F) -> (a,b,c,alpha,beta,gamma)
    Eigen::MatrixXd J(6, 6);
    J.setZero();    

    // Jacobian entries for a,b,c
    for (int i = 0; i < 3; ++i) {
        J(i,i) = 1 / 2.0 / abc[i];
    }

    // Jacobian entries for alpha,beta,gamma
    // e.g. alpha = acos(b*c/D)
    // so we calculate the derivative analytically
    for (int i = 3; i < 6; ++i) {
        // derivative of inverse cosine
        const double factor = -1.0 / std::sin(abc[i]);

        // shift of index, used for convenience
        const int i1 = (i-3+1)%3;
        const int i2 = (i-3+2)%3;

        const double Dbc = ABC[i] / abc[i1] / abc[i2];

        // diagonal part of Jacobian, e.g. d(alpha) / dD
        J(i, i) = -factor / abc[i1] / abc[i2];

        // off-diagonal part of Jacobian, e.g.
        // d(alpha)/dB and d(alpha)/dC
        J(i,i1) = factor * Dbc / 2.0 / ABC[i1];
        J(i,i2) = factor * Dbc / 2.0 / ABC[i2];
        
    }

    // covariance matrix of the paramters a, b, c, alpha, beta, gamma
    Eigen::MatrixXd abc_cov = J * ABC_cov * J.transpose();

    // store the result
    _characterSigmas.A = std::sqrt(ABC_cov(0,0));
    _characterSigmas.B = std::sqrt(ABC_cov(1,1));
    _characterSigmas.C = std::sqrt(ABC_cov(2,2));
    _characterSigmas.D = std::sqrt(ABC_cov(3,3));
    _characterSigmas.E = std::sqrt(ABC_cov(4,4));
    _characterSigmas.F = std::sqrt(ABC_cov(5,5));

    _characterSigmas.a = std::sqrt(abc_cov(0,0));
    _characterSigmas.b = std::sqrt(abc_cov(1,1));
    _characterSigmas.c = std::sqrt(abc_cov(2,2));
    _characterSigmas.alpha = std::sqrt(abc_cov(3,3));
    _characterSigmas.beta = std::sqrt(abc_cov(4,4));
    _characterSigmas.gamma = std::sqrt(abc_cov(5,5));
}

std::vector<std::string> UnitCell::compatibleSpaceGroups() const
{
    std::vector<std::string> compatible_space_groups;

    auto cell_bravais_type = bravaisTypeSymbol();

    for (auto&& symbol : SpaceGroup::symbols()) {
        SpaceGroup sg(symbol);
        if (sg.bravaisTypeSymbol().compare(cell_bravais_type) == 0) {
            compatible_space_groups.push_back(symbol);
        }
    }

    return compatible_space_groups;
}

} // end namespace nsx
