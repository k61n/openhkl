#include <cmath>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "DataSet.h"
#include "GruberReduction.h"
#include "Logger.h"
#include "MillerIndex.h"
#include "Minimizer.h"
#include "NiggliReduction.h"
#include "ReciprocalVector.h"
#include "UnitCell.h"
#include "Units.h"

namespace nsx {

UnitCell UnitCell::interpolate(const UnitCell &uc1, const UnitCell &uc2, double t)
{
    if (uc1._niggli != uc2._niggli) {
        throw std::runtime_error("Inconsistent Niggli type");
    }

    if (uc1._bravaisType != uc2._bravaisType) {
        throw std::runtime_error("Inconsistent Bravais type");
    }

    if (uc1._centring != uc2._centring) {
        throw std::runtime_error("Inconsistent centring type");
    }

    const double s = 1.0-t;

    const Eigen::Quaterniond uc1_u_quat(uc1.orientation());
    const Eigen::Quaterniond uc2_u_quat(uc2.orientation());

    Eigen::Quaterniond u_quat = uc1_u_quat.slerp(t,uc2_u_quat);
    u_quat.normalize();
    const Eigen::Matrix3d u_matrix = u_quat.toRotationMatrix();

    auto&& uc1_params = uc1.character();
    auto&& uc2_params = uc2.character();

    const double g00 = s*uc1_params.g00 + t*uc2_params.g00;
    const double g01 = s*uc1_params.g01 + t*uc2_params.g01;
    const double g02 = s*uc1_params.g02 + t*uc2_params.g02;
    const double g11 = s*uc1_params.g11 + t*uc2_params.g11;
    const double g12 = s*uc1_params.g12 + t*uc2_params.g12;
    const double g22 = s*uc1_params.g22 + t*uc2_params.g22;

    // create new unit cell
    UnitCell uc(uc1);
    uc.setMetric(g00,g01,g02,g11,g12,g22);

    Eigen::MatrixXd kernel;

    // no constraints
    if (uc1._niggli.number == 31 || uc1._niggli.number == 44) {
        kernel.setIdentity(6, 6);
    } else {
        // matrix of Niggli character constraints, taken from the table 9.2.5.1
        Eigen::MatrixXd C = uc._niggli.C;
        // compute kernel of Niggli constraints
        Eigen::FullPivLU<Eigen::MatrixXd> lu(C);
        kernel = lu.kernel();
    }

    auto&& uc_params = uc.character();
    Eigen::VectorXd parameters(6);
    parameters(0) = uc_params.g00;
    parameters(5) = uc_params.g01;
    parameters(4) = uc_params.g02;
    parameters(1) = uc_params.g11;
    parameters(3) = uc_params.g12;
    parameters(2) = uc_params.g22;

    const int nparams = kernel.cols();

    // lattice character
    Eigen::VectorXd ch(6);
    ch.setZero();
    // parameters defining lattice chatacer
    for (auto i = 0; i < nparams; ++i) {
        ch += parameters(i)*kernel.col(i);
    }

    uc.setBasis(u_matrix*uc._a*uc._NP);

    return uc;
}

UnitCellCharacter::UnitCellCharacter()
: g00(0.0),
  g01(0.0),
  g02(0.0),
  g11(0.0),
  g12(0.0),
  g22(0.0),
  a(0.0),
  b(0.0),
  c(0.0),
  alpha(0.0),
  beta(0.0),
  gamma(0.0)
{
}

UnitCellCharacter::UnitCellCharacter(const Eigen::Matrix3d& g)
: UnitCellCharacter(g(0,0), g(0,1), g(0,2), g(1,1), g(1,2), g(2,2))
{
}

UnitCellCharacter::UnitCellCharacter(double g00_, double g01_, double g02_, double g11_, double g12_, double g22_)
{
    g00 = g00_;
    g01 = g01_;
    g02 = g02_;
    g11 = g11_;
    g22 = g22_;
    g12 = g12_;

    a = std::sqrt(g00);
    b = std::sqrt(g11);
    c = std::sqrt(g22);

    alpha = std::acos(g12 / b / c);
    beta  = std::acos(g02 / a / c);
    gamma = std::acos(g01 / a / b);
}

UnitCell::UnitCell(const Eigen::Matrix3d& basis, bool reciprocal): UnitCell()
{
    // If reciprocal is false basis must be column formed, upper triangular
    _a = reciprocal ? basis.inverse() : basis;
    // If reciprocal is true basis must be row formed, lower triangular
    _b_transposed = reciprocal ? basis : basis.inverse();
}

UnitCell::UnitCell()
: _a(Eigen::Matrix3d::Identity()),
  _b_transposed(Eigen::Matrix3d::Identity()),
  _NP(Eigen::Matrix3d::Identity()),
  _material(nullptr),
  _centring(LatticeCentring::P),
  _bravaisType(BravaisType::Triclinic),
  _Z(1),
  _space_group("P 1"),
  _name("uc"),
  _indexingTolerance(0.2),
  _niggli()
{
}

UnitCell::UnitCell(const UnitCell &other)
: _a(other._a),
  _b_transposed(other._b_transposed),
  _NP(other._NP),
  _centring(other._centring),
  _bravaisType(other._bravaisType),
  _Z(other._Z),
  _space_group(other._space_group),
  _name(other._name),
  _indexingTolerance(other._indexingTolerance),
  _niggli(other._niggli),
  _characterSigmas(other._characterSigmas),
  _states(other._states)
{
    _material.reset(other._material ? other._material->clone() : nullptr);
}


UnitCell::UnitCell(double a, double b, double c, double alpha, double beta, double gamma): UnitCell()
{
    setParameters(a,b,c,alpha,beta,gamma);
}

UnitCell& UnitCell::operator=(const UnitCell &other)
{
    if (this != &other) {
        _a = other._a;
        _b_transposed = other._b_transposed;
        _NP = other._NP;
        _material.reset(other._material ? other._material->clone() : nullptr);
        _centring = other._centring;
        _bravaisType = other._bravaisType;
        _Z = other._Z;
        _space_group = other._space_group;
        _name = other._name;
        _indexingTolerance = other._indexingTolerance;
        _niggli = other._niggli;
        _characterSigmas = other._characterSigmas;
        _states = other._states;
    }
    return *this;
}

void UnitCell::initState(sptrDataSet data) {

    auto it = _states.find(data);
    if (it != _states.end()) {
        return;
    }

    std::vector<UnitCellState> states;
    auto n_frames = data->nFrames();
    states.reserve(n_frames);

    auto u_matrix = orientation();
    auto ch = character();

    for (size_t i=0; i < n_frames; ++i) {
        states.push_back({u_matrix,ch});
    }
}

UnitCellState& UnitCell::state(sptrDataSet data, size_t frame) {

    auto it = _states.find(data);
    if (it == _states.end()) {
        initState(data);
    }

    auto&& states = _states[data];

    if (frame > (states.size()-1) || frame < 0) {
        throw std::runtime_error("UnitCell::setState: Invalid frame number");
    }

    return states[frame];
}

void UnitCell::setState(sptrDataSet data, size_t frame, const UnitCellState& state)
{
    auto it = _states.find(data);
    if (it == _states.end()) {
        initState(data);
    }

    auto&& states = _states[data];

    if (frame > (states.size()-1) || frame < 0) {
        throw std::runtime_error("UnitCell::setState: Invalid frame number");
    }

    states[frame] = state;
}

UnitCell UnitCell::interpolate(sptrDataSet data, double frame)
{
    auto it = _states.find(data);
    if (it == _states.end()) {
        throw std::runtime_error("No unit cells states stored for this dataset.");
    }

    auto&& states = it->second;

    auto n_frames = data->nFrames();

    if (n_frames != states.size()) {
        throw std::runtime_error("Inconsistent state");
    }

    if (frame > (n_frames-1) || frame < 0) {
        throw std::runtime_error("Error when interpolating state: invalid frame value: " + std::to_string(frame));
    }

    const std::size_t idx = std::size_t(std::lround(std::floor(frame)));
    const std::size_t next = std::min(idx+1, n_frames-1);

    auto&& state1 = states[idx];
    auto&& state2 = states[next];

    const double t = frame-idx;

    const Eigen::Quaterniond uc1_u_quat(state1.orientation);
    const Eigen::Quaterniond uc2_u_quat(state2.orientation);

    Eigen::Quaterniond u_quat = uc1_u_quat.slerp(t,uc2_u_quat);
    u_quat.normalize();
    const Eigen::Matrix3d u_matrix = u_quat.toRotationMatrix();

    auto&& character1 = state1.character;
    auto&& character2 = state2.character;

    const double s = 1.0-t;

    const double g00 = s*character1.g00 + t*character2.g00;
    const double g01 = s*character1.g01 + t*character2.g01;
    const double g02 = s*character1.g02 + t*character2.g02;
    const double g11 = s*character1.g11 + t*character2.g11;
    const double g12 = s*character1.g12 + t*character2.g12;
    const double g22 = s*character1.g22 + t*character2.g22;

    // create new unit cell
    UnitCell uc(*this);
    uc.setMetric(g00,g01,g02,g11,g12,g22);

    Eigen::MatrixXd kernel;

    // no constraints
    if (_niggli.number == 31 || _niggli.number == 44) {
        kernel.setIdentity(6, 6);
    } else {
        // matrix of Niggli character constraints, taken from the table 9.2.5.1
        Eigen::MatrixXd C = uc._niggli.C;
        // compute kernel of Niggli constraints
        Eigen::FullPivLU<Eigen::MatrixXd> lu(C);
        kernel = lu.kernel();
    }

    auto&& uc_params = uc.character();
    Eigen::VectorXd parameters(6);
    parameters(0) = uc_params.g00;
    parameters(5) = uc_params.g01;
    parameters(4) = uc_params.g02;
    parameters(1) = uc_params.g11;
    parameters(3) = uc_params.g12;
    parameters(2) = uc_params.g22;

    const int nparams = kernel.cols();

    // lattice character
    Eigen::VectorXd ch(6);
    ch.setZero();
    // parameters defining lattice character
    for (auto i = 0; i < nparams; ++i) {
        ch += parameters(i)*kernel.col(i);
    }

    uc.setBasis(u_matrix*uc._a*uc._NP);

    return uc;
}

void UnitCell::setParameters(double a, double b, double c, double alpha, double beta, double gamma)
{
//    const double ca = std::cos(alpha);
//    const double sa = std::sin(alpha);
//
//    const double cb = std::cos(beta);
//    const double sb = std::sin(beta);
//
//    const double cg = std::cos(gamma);
//    const double sg = std::sin(gamma);
//
//    const double metric_factor = std::sqrt(1.0 - ca*ca - cb*cb - cg*cg + 2.0*ca*cb*cg);
//
//    const double as = sa/metric_factor/a;
//    const double bs = sb/metric_factor/b;
//    const double cs = sg/metric_factor/c;
//
//    const double cbs = (ca*cg - cb)/sa/sg;
//    const double cgs = (ca*cb - cg)/sa/sb;
//
//    const double sbs = std::sin(std::acos(cbs));
//    const double sgs = std::sin(std::acos(cgs));
//
//    _b_transposed << as    ,         0,   0,
//                     bs*cgs,    bs*sgs,   0,
//                     cs*cbs,-cs*sbs*ca, 1/c;
//
//    _a = _b_transposed.inverse();

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

    _a << a1, b1, c1,
           0, b2, c2,
           0,  0, c3;

    _b_transposed = _a.inverse();
}

void UnitCell::setReciprocalParameters(double as, double bs, double cs, double alphas, double betas, double gammas)
{
    const double cas = std::cos(alphas);

    const double cbs = std::cos(betas);
    const double sbs = std::sin(betas);

    const double cgs = std::cos(gammas);
    const double sgs = std::sin(gammas);

    const double ca = (cbs*cgs - cas)/sbs/sgs;

    const double metric_factor = std::sqrt(1.0 - cas*cas - cbs*cbs - cgs*cgs + 2.0*cas*cbs*cgs);

    const double c = sgs/metric_factor/cs;

    _b_transposed << as, bs*cgs,     cs*cbs,
                      0, bs*sgs, -cs*sbs*ca,
                      0,      0,        1/c;


    _a = _b_transposed.inverse();
}

void UnitCell::setMetric(double g00, double g01, double g02, double g11, double g12, double g22)
{
    // make sure the parameters are in the valid range
    g00 = std::fabs(g00);
    g11 = std::fabs(g11);
    g22 = std::fabs(g22);

    double a, b, c, alpha, beta, gamma;
    a = std::sqrt(g00);
    b = std::sqrt(g11);
    c = std::sqrt(g22);

    // more checking
    if (std::fabs(g12) > b*c) {
        g12 = (std::signbit(g12) ? -1 : 1) * b * c;
    }
    if (std::fabs(g02) > a*c) {
        g02 = (std::signbit(g02) ? -1 : 1) * a * c;
    }
    if (std::fabs(g01) > a*b) {
        g01 = (std::signbit(g01) ? -1 : 1) * a * b;
    }

    alpha = std::acos(g12 / b / c);
    beta  = std::acos(g02 / a / c);
    gamma = std::acos(g01 / a / b);

    setParameters(a, b, c, alpha, beta, gamma);
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
    return 1.0 / q.norm();
}


void UnitCell::printSelf(std::ostream& os) const
{
    auto c = character();
    auto rc = reciprocalCharacter();

    os << "Space group:" << std::endl;
    os << _space_group << std::endl;
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
    os << _b_transposed << std::endl;
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

std::vector<MillerIndex> UnitCell::generateReflectionsInShell(double dmin, double dmax, double wavelength) const
{
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigen_solver;
    std::vector<MillerIndex> hkls;
    eigen_solver.compute(reciprocalMetric());
    double b_min = std::sqrt(eigen_solver.eigenvalues().minCoeff());
    const int hkl_max = std::ceil(2.0 / (wavelength * b_min)); // maximum allowed by Bragg law
    const int num_hkl = 2*hkl_max+1;
    hkls.reserve(num_hkl*num_hkl*num_hkl);

    for (int h = -hkl_max; h <= hkl_max; ++h) {

        for (int k = -hkl_max; k <= hkl_max; ++k) {

            for (int l = -hkl_max; l <= hkl_max; ++l) {

                // Always skip the (0,0,0) which is irrelevant
                if (h == 0 && k == 0 && l == 0) {
                    continue;
                }

                MillerIndex hkl(h, k, l);
                Eigen::RowVector3d q = hkl.rowVector().cast<double>()*_b_transposed;
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
                if (_space_group.isExtinct(hkl)) {
                    continue;
                }

                hkls.emplace_back(hkl);
            }
        }
    }

    nsx::info() << "Generated "<<hkls.size()<<" hkl in ["<<dmin<<","<<dmax<<"] range";

    return hkls;
}


double UnitCell::angle(const Eigen::RowVector3d& hkl1, const Eigen::RowVector3d& hkl2) const
{
    auto q1=hkl1*_b_transposed;
    auto q2=hkl2*_b_transposed;

    // Safe guard for avoiding nan with acos
    double a_dot_b_over_ab = q1.dot(q2)/q1.norm()/q2.norm();
    a_dot_b_over_ab = std::min(a_dot_b_over_ab, 1.0);
    a_dot_b_over_ab = std::max(a_dot_b_over_ab,-1.0);

    return std::acos(a_dot_b_over_ab);
}

unsigned int UnitCell::z() const
{
    return _Z;
}

void UnitCell::setZ(unsigned int Z)
{
    _Z = Z;
}

Material* UnitCell::material()
{
    return _material.get();
}

const Material* UnitCell::material() const
{
    return _material.get();
}

void UnitCell::setMaterial(std::unique_ptr<Material> material)
{
    _material = std::move(material);
}

void UnitCell::setMaterial(const Material& material)
{
    _material.reset(material.clone());
}

void UnitCell::setSpaceGroup(const SpaceGroup& space_group)
{
    _space_group = _space_group;
}

const SpaceGroup& UnitCell::spaceGroup() const
{
    return _space_group;
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
//! find A, B = A^{-1} such that the L2 matrix norm |B-B0|^2 is minimized. The reason for choosing |B-B0|^2
//! as the objective function is so that the resulting predicted q values will be as close as possible.
UnitCell UnitCell::applyNiggliConstraints() const
{
    // no constraints for these cases to early return
    if (_niggli.number == 31 || _niggli.number == 44) {
        return *this;
    }
    
    // geometric mean of side-lengths of unit cell & reciprocal unit cell
    // we use these to scale the residuals in the fitting function below   
    const double b = std::pow(std::fabs(_b_transposed.determinant()), 1.0/3.0);

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
        Eigen::Matrix3d B = uc.reciprocalBasis();

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                residuals(3*i+j) = (B(i,j) - _b_transposed(i, j)) / b;
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

    min.initialize(params, 9);
    min.set_f(functor);

    min.setxTol(1e-15);
    min.setfTol(1e-15);
    min.setgTol(1e-15);

    // note: if the UC already satisfies the constraints, the minimizer will fail with GSL_ENOPROG
    // so we don't check the return value of Minimizer::fit
    min.fit(1000);
    nsx::UnitCell new_uc = fromParameters(U, uOffset, p);

    // check if the new UC is close to the old one
    const double delta = (new_uc.reciprocalBasis()-_b_transposed).norm() / _b_transposed.norm();

    if (delta < 0.1) {
        return new_uc;
    } 
    throw std::runtime_error("ERROR: could not apply symmetry constraints to unit cell");
}

Eigen::RowVector3d UnitCell::index(const ReciprocalVector& q) const
{
    return q.rowVector()*_a;
}

Eigen::RowVector3d UnitCell::fromIndex(const Eigen::RowVector3d& hkl) const
{
    return hkl*_b_transposed;
}
    
UnitCellCharacter UnitCell::character() const
{
    return UnitCellCharacter(metric());
}

UnitCellCharacter UnitCell::characterSigmas() const
{   
    return _characterSigmas;
}

UnitCellCharacter UnitCell::reciprocalCharacter() const
{
    return UnitCellCharacter(reciprocalMetric());
}

double UnitCell::volume() const
{
    return std::fabs(_a.determinant());
}

Eigen::Matrix3d UnitCell::metric() const
{
    return _a.transpose()*_a;
}

Eigen::Matrix3d UnitCell::reciprocalMetric() const
{
    return _b_transposed*_b_transposed.transpose();
}

const Eigen::Matrix3d& UnitCell::basis() const
{
    return _a;
}

const Eigen::Matrix3d& UnitCell::reciprocalBasis() const
{
    return _b_transposed;
}

void UnitCell::setReciprocalBasis(const Eigen::Matrix3d& b_transposed)
{
    _b_transposed = b_transposed;
    _a = b_transposed.inverse();
}

void UnitCell::transform(const Eigen::Matrix3d& P)
{
    _a = _a*P;
    _b_transposed = _a.inverse();
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

    if (niggli_only) {
        _NP = Eigen::Matrix3d::Identity();
    } else {
        transform(P);
        _NP = P;
    }
    return _niggli.number;
}

void UnitCell::setBasis(const Eigen::Matrix3d& a)
{
    _a = a;
    _b_transposed = a.inverse();
}

const NiggliCharacter& UnitCell::niggliCharacter() const
{
    return _niggli;
}

Eigen::Matrix3d UnitCell::niggliBasis() const
{
    return _a*_NP.inverse();
}

Eigen::Matrix3d UnitCell::reciprocalNiggliBasis() const
{
    return _NP*_b_transposed;
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
    Eigen::Matrix3d Q = _a.householderQr().householderQ();
    Eigen::Matrix3d R = Q.transpose() * _a;

    for (auto i = 0; i < 3; ++i) {
        if (R(i,i) < 0) {
            Q.col(i) *= -1.0;
        }
    }
    return Q;
}

Eigen::Matrix3d UnitCell::niggliOrientation() const
{
    const Eigen::Matrix3d NA = _a*_NP.inverse();
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
    Eigen::Matrix3d A0 = _a*_NP.inverse();
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
    uc.setMetric(ch(0), ch(5), ch(4), ch(1), ch(3), ch(2));
    uc.setBasis(U*uc._a*_NP);

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
    auto ch = character();
    // store character in these arrays to make symbolic calculation easier
    const double ABC[6] = {ch.g00, ch.g11, ch.g22, ch.g12, ch.g02, ch.g01};
    const double abc[6] = {ch.a, ch.b, ch.c, ch.alpha, ch.beta, ch.gamma}; 

    // Jacobian of the transformation (g00,g01,g02,g11,g12,g22) -> (a,b,c,alpha,beta,gamma)
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
    _characterSigmas.g00 = std::sqrt(ABC_cov(0,0));
    _characterSigmas.g01 = std::sqrt(ABC_cov(5,5));
    _characterSigmas.g02 = std::sqrt(ABC_cov(4,4));
    _characterSigmas.g11 = std::sqrt(ABC_cov(1,1));
    _characterSigmas.g12 = std::sqrt(ABC_cov(3,3));
    _characterSigmas.g22 = std::sqrt(ABC_cov(2,2));

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
