//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/UnitCell.h
//! @brief     Defines class UnitCell
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_TABLES_CRYSTAL_UNITCELL_H
#define OHKL_TABLES_CRYSTAL_UNITCELL_H

#include "tables/crystal/NiggliCharacter.h"
#include "tables/crystal/SpaceGroup.h"

#include <memory>

namespace ohkl {

using sptrUnitCell = std::shared_ptr<UnitCell>;

class MillerIndex;

//! Structure to encapsulate lattice cell character.

struct UnitCellCharacter {
    //! Component (0,0) of the metric tensor
    double g00;
    //! Component (0,1) of the metric tensor
    double g01;
    //! Component (0,2) of the metric tensor
    double g02;
    //! Component (1,1) of the metric tensor
    double g11;
    //! Component (1,2) of the metric tensor
    double g12;
    //! Component (2,2) of the metric tensor
    double g22;
    //! Lattice character \f$a = |\mathbf{a}|\f$
    double a;
    //! Lattice character \f$b = |\mathbf{b}|\f$
    double b;
    //! Lattice character \f$c = |\mathbf{c}|\f$
    double c;
    //! Lattice character \f$\alpha = \angle(\mathbf{b}, \mathbf{c})\f$
    double alpha;
    //! Lattice character \f$\beta = \angle(\mathbf{a}, \mathbf{c})\f$
    double beta;
    //! Lattice character \f$\gamma = \angle(\mathbf{a}, \mathbf{b})\f$
    double gamma;

    //! Default constructor: set all parameters to zero.
    UnitCellCharacter();
    //! Construct a lattice character from a given metric tensor $g$.
    UnitCellCharacter(const Eigen::Matrix3d& g);
    //! Construct a lattice character from the given metric components.
    UnitCellCharacter(double g00_, double g01_, double g02_, double g11_, double g12_, double g22_);
};

//! Class to define a crystallographic unit cell.

//! Provide functionalities to transform vectors of the direct lattice or
//! reciprocal lattice from unit-cell coordinates to a right-handed coordinates
//! system. The UnitCell is defined by the parameters a,b,c and angles alpha,
//! beta, gamma. The UnitCell parameters a,b,c are given in \f$ \AA \f$  and
//! angle alpha, beta, gamma are given in degrees. The a axis is chosen as
//! pointing along the x-direction, and the b-axis is in the xy-plane.

class UnitCell {
 public:
    //! Empty UnitCell, initialized to right-handed orthonormal system
    UnitCell();

    //! Create unit cell from a basis
    UnitCell(const Eigen::Matrix3d& basis_, bool reciprocal = false);

    //! Construct unitCell from lattice parameters, the A matrix is built with
    //! avector along x, bvector in the xy-plane.
    UnitCell(double a, double b, double c, double alpha, double beta, double gamma);
    UnitCell(const UnitCell& other);

    ~UnitCell() = default;

    UnitCell& operator=(const UnitCell& other);

    //! Sets lattice parameters
    void setParameters(double a, double b, double c, double alpha, double beta, double gamma);

    //! Sets reciprocal lattice parameters
    void setReciprocalParameters(
        double as, double bs, double cs, double alphas, double betas, double gammas);

    //! Sets the reciprocal space basis (lower triangular row form)
    void setReciprocalBasis(const Eigen::Matrix3d& b_transposed);

    //! Sets lattice centering type
    void setLatticeCentring(LatticeCentring centring);
    //! Return the centering condition
    char latticeCentering() const { return (char)_centring; };

    //! Sets Bravais type
    void setBravaisType(BravaisType bravais);

    //! Returns Bravais type symbol
    std::string bravaisTypeSymbol() const;

    //! Returns a list of reflections with d value in the range [dmin, dmax]
    std::vector<MillerIndex> generateReflectionsInShell(
        double dmin, double dmax, double wavelength) const;

    //! Returns the angle in radians between two reflections hkl1 and hkl2
    double angle(const Eigen::RowVector3d& hkl1, const Eigen::RowVector3d& hkl2) const;

    //! Sets the Niggli character of the cell
    void setNiggli(const NiggliCharacter& niggli);

    //! Returns the Niggli character of the cell
    const NiggliCharacter& niggliCharacter() const;

    //! Returns the basis for the Niggli cell (assuming cell has already been reduced)
    Eigen::Matrix3d niggliBasis() const;

    //! Returns the basis for the Niggli cell (assuming cell has already been reduced)
    Eigen::Matrix3d reciprocalNiggliBasis() const;

    //! Apply constraint from the Niggli character
    UnitCell applyNiggliConstraints() const;

    //! Returns d of the Bragg condition for the given reflection
    double d(int h, int k, int l) const;

    //! Print into a stream
    void printSelf(std::ostream& os) const;

    //! Print a summary of the cell parameters
    std::string toString() const;

    //! Returns the Z value for the unit cell
    unsigned int z() const;

    //! Sets the Z value for the unit cell
    void setZ(unsigned int Z);

    //! Sets space group from its symbol.
    void setSpaceGroup(const SpaceGroup& space_group);

    //! Sets space group from its symbol.
    void setSpaceGroup(const std::string symbol);

    //! Returns the space group symbol of the unit cell.
    const SpaceGroup& spaceGroup() const;

    //! Sets name of the unit cell
    void setName(const std::string& name);

    //! Returns the name
    std::string name() const;

    //! Sets the integer tolerance for this unit cell
    void setIndexingTolerance(double tolerance);

    //! Returns the integer tolerance for this unit cell
    double indexingTolerance() const;

    //! Returns the real-space basis
    const Eigen::Matrix3d& basis() const;

    //! Sets the real-space basis (upper triangular, column form)
    void setBasis(const Eigen::Matrix3d& a);

    //! Returns the reciprocal bases
    const Eigen::Matrix3d& reciprocalBasis() const;

    //! Sets lattice parameters from metric tensor
    void setMetric(double g00, double g01, double g02, double g11, double g12, double g22);

    //! Returns the real space metric tensor
    Eigen::Matrix3d metric() const;

    //! Returns the reciprocal space metric tensor
    Eigen::Matrix3d reciprocalMetric() const;

    //! Returns the volume of the unit cell
    double volume() const;

    //! Returns the index of a given q vector (not necessarily integral!!)
    Eigen::RowVector3d index(const ReciprocalVector& q) const;

    //! Returns q vector from a given hkl
    Eigen::RowVector3d fromIndex(const Eigen::RowVector3d& hkl) const;

    //! Returns the character of the cell
    UnitCellCharacter character() const;

    //! Returns the errors in the character of the cell
    UnitCellCharacter characterSigmas() const;

    //! Returns the reciprocal character of the cell
    UnitCellCharacter reciprocalCharacter() const;

    //! Reduce the unit cell to Niggli or conventional cell. Returns the number
    //! according to the classification into 44 lattice types.
    int reduce(bool niggli_only, double niggliTolerance, double gruberTolerance);

    //! transform the unit cell (perform a change of basis)
    //! Note: in the future it might be better to make this method private
    void transform(const Eigen::Matrix3d& P);

    //! Check if two unit cells are equivalent, within tolerance.
    //! Equivalence means that there exist matrices T, S with integral entries
    //! such that ST = 1 and A1 = A0*T, A0 = A1*S
    bool equivalent(const UnitCell& other, double tolerance) const;

    //! Returns the matrix P transforming the Niggli cell to the conventional cell
    const Eigen::Matrix3d& niggliTransformation() const;

    //! Returns the orientation matrix Q such that _A = Q*R where R is
    //! upper triangular with positive entries on the diagonal, i.e.
    //! transformation mapping crystal space into real space.
    Eigen::Matrix3d orientation() const;

    //! Returns the orientation matrix Q such that _A*_NP^{-1} = Q*R where R is
    //! upper triangular with positive entries on the diagonal. This is similar to
    //! UnitCell::orientation() except that the orientation is computed for the Niggli cell.
    Eigen::Matrix3d niggliOrientation() const;

    //! Return parameters of the unit cell in an internal format.
    Eigen::VectorXd parameters() const;

    //! Set the uncertainty in the cell parameters.
    //! We use the parameter uncertainty and propagation of error to estimate the
    //! uncertainty in the cell parameters
    // \f$a\f$,\f$b\f$,\f$c\f$,\f$\alpha\f$,\f$\beta\f$,\f$\gamma\f$.
    void setParameterCovariance(const Eigen::MatrixXd& cov);

    //! Construct a new unit cell from a reference orientation, an
    //! orientation offset, and a set of parameters.
    UnitCell fromParameters(
        const Eigen::Matrix3d& U0, const Eigen::Vector3d& uOffset,
        const Eigen::VectorXd& parameters) const;

    //! Construct a new unit cell from a reference orientation, an
    //! orientation offset, and a set of parameters.
    void updateParameters(
        const Eigen::Matrix3d& U0, const Eigen::Vector3d& uOffset,
        const Eigen::VectorXd& parameters);

    //! Returns list of space groups which are compatible with the Bravais type of the cell
    std::vector<std::string> compatibleSpaceGroups() const;

    //! Return true if similar to another unit cell
    bool isSimilar(const UnitCell* other, double length_tol, double angle_tol) const;

    unsigned int id() const;
    void setId(const unsigned int id);

#ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

 private:
    //! Real-space basis of column vectors
    Eigen::Matrix3d _a;
    //! Reciprocal-space basis of row vectors
    Eigen::Matrix3d _b_transposed;
    //! _NP is the transformation such that _A*_NP.inverse() is the Niggli cell
    Eigen::Matrix3d _NP;

    LatticeCentring _centring;

    BravaisType _bravaisType;

    unsigned int _Z;

    SpaceGroup _space_group;

    std::string _name;

    double _indexingTolerance;

    NiggliCharacter _niggli;

    UnitCellCharacter _characterSigmas;

    unsigned int _id;
};

//! Print to a stream
std::ostream& operator<<(std::ostream&, const UnitCell&);

using sptrUnitCell = std::shared_ptr<UnitCell>;
using UnitCellList = std::vector<sptrUnitCell>;

} // namespace ohkl

#endif // OHKL_TABLES_CRYSTAL_UNITCELL_H
