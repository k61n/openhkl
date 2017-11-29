/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include <vector>
#include <string>

#include <Eigen/Dense>

#include "ChemistryTypes.h"
#include "GeometryTypes.h"
#include "NiggliCharacter.h"
#include "SpaceGroup.h"

namespace nsx {

    /*
    struct MillerIndex {
        // todo: think about this....
        const Eigen::RowVector3d hkl; // rounded
        const Eigen::RowVector3d delta_hkl; // error from raw hkl
        //bool commensurate(double tolerance) const;
        //Eigen::RowVector3d getIntegralIndex(double tolerance) const;
    };*/

    //! \brief Structure to encapsulate lattice cell character.
    struct CellCharacter {
        //! Lattice character \f$A = \mathbf{a} \cdot \mathbf{a}\f$
        double A;
        //! Lattice character \f$B = \mathbf{b} \cdot \mathbf{b}\f$
        double B;
        //! Lattice character \f$C = \mathbf{c} \cdot \mathbf{c}\f$
        double C;
        //! Lattice character \f$D = \mathbf{b} \cdot \mathbf{c}\f$
        double D;
        //! Lattice character \f$E = \mathbf{a} \cdot \mathbf{c}\f$
        double E;
        //! Lattice character \f$F = \mathbf{a} \cdot \mathbf{b}\f$
        double F;
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
        CellCharacter();
        //! Construct a lattice character from a given metric tensor $g$.
        CellCharacter(const Eigen::Matrix3d& g);
        //! Construct a lattice character from the given metric components.
        CellCharacter(double A, double B, double C, double D, double E, double F);
    };

//! \brief Class to define a crystallographic unit-cell.
//!
//! Provide functionalities to transform vectors of the direct lattice or reciprocal
//! lattice from unit-cell coordinates to a right-handed coordinates system.
//! The UnitCell is defined by the parameters a,b,c and angles alpha, beta, gamma.
//! The UnitCell parameters a,b,c are given in \f$ \AA \f$  and angle alpha, beta, gamma
//! are given in degrees.
//! The a axis is chosen as pointing along the x-direction, and the b-axis is in the xy-plane.
class UnitCell 
{
public:
    //! Empty UnitCell, initialiazed to right-handed orthonormal system
    UnitCell();
    //! Create unit cell from a basis
    UnitCell(const Eigen::Matrix3d& basis_, bool reciprocal=false);
    //! Construct unitCell from lattice parameters, the A matrix is built with avector along x, bvector in the xy-plane.
    UnitCell(double a, double b, double c, double alpha, double beta, double gamma);

    //! Copy constructor
    UnitCell(const UnitCell& other) = default;
  
    //! Set lattice parameters
    void setParams(double a, double b, double c, double alpha, double beta, double gamma);

    //! Set lattice parmeters from ABCDEF
    void setABCDEF(double A, double B, double C, double D, double E, double F);

    //! Set the reciprocal basis
    void setReciprocalBasis(const Eigen::Matrix3d& B);
    
    //! Set lattice centering type
    void setLatticeCentring(LatticeCentring centring);
    //! Set Bravais type
    void setBravaisType(BravaisType bravais);
    //! Get Bravais type symbol
    std::string getBravaisTypeSymbol() const;
    
    //! Get a list of reflections with d value in the range [dmin, dmax]
    std::vector<Eigen::RowVector3d> generateReflectionsInShell(double dmin, double dmax, double wavelength) const;

    //! Return the angle in radians between two reflections hkl1 and hkl2
    double getAngle(const Eigen::RowVector3d& hkl1, const Eigen::RowVector3d& hkl2) const;

    //! Determine whether two reflections \f$(h_1,k_1,l_1)\f$ and \f$(h_2,k_2,l_2)\f$ are equivalent.
    bool isEquivalent(double h1,double k1, double l1, double h2, double k2, double l2) const;
    //! Determine whether two reflections (h1,k1,l1) and (h2,k2,l2) are equivalent up to Friedel reflection.
    bool isFriedelEquivalent(double h1,double k1, double l1, double h2, double k2, double l2) const;

    //! Set the Niggli character of the cell
    void setNiggli(const NiggliCharacter& niggli);
    //! Get the Niggli character of the cell
    const NiggliCharacter& niggliCharacter() const;

    //! Return the basis for the Niggli cell (assuming cell has already been reduced)
    Eigen::Matrix3d niggliBasis() const;
    //! Return the basis for the Niggli cell (assuming cell has already been reduced)
    Eigen::Matrix3d reciprocalNiggliBasis() const;

    //! Apply constraint from the Niggli character
    UnitCell applyNiggliConstraints() const;

    //! Return d of the Bragg condition for the given reflection
    double getD(int h, int k, int l);

    //! Print into a stream
    void printSelf(std::ostream& os) const;

    //! Gets the Z value for the unit cell
    unsigned int getZ() const;
    //! Sets the Z value for the unit cell
    void setZ(unsigned int Z);

    //! Gets the Material for the unit cell
    sptrMaterial getMaterial() const;
    //! Sets the Material for the unit cell
    void setMaterial(const sptrMaterial& material);
    //! Set space group from its symbol.
    void setSpaceGroup(const std::string& symbol);
    //! Return the space group symbol of the unit cell.
    std::string getSpaceGroup() const;

    // todo: should move name to GUI??
    //! Set name of the unit cell
    void setName(const std::string& name);
    //! Get name of the unit cell
    const std::string& getName() const;

    // move to GUI??
    //! Set the integer tolerance for this unit cell
    void setHKLTolerance(double tolerance);
    //! Get the integer tolerance for this unit cell
    double getHKLTolerance() const;

    //! Return the real-space basis
    const Eigen::Matrix3d& basis() const;
    //! Set the real-space basis
    void setBasis(const Eigen::Matrix3d& b);
    //! Return the reciprocal bases
    const Eigen::Matrix3d& reciprocalBasis() const;
    //! Return the real space metric tensor
    Eigen::Matrix3d metric() const;
    //! Return the reciprocal space metric tensor
    Eigen::Matrix3d reciprocalMetric() const;
    //! Return the volume of the unit cell
    double volume() const;

    //! Return the index of a given q vector (not necessarily integral!!)
    Eigen::RowVector3d index(const ReciprocalVector& q) const;
    //! Return q vector from a given hkl
    Eigen::RowVector3d fromIndex(const Eigen::RowVector3d& hkl) const;

    //! Return the character of the cell
    CellCharacter character() const;
    //! Return the errors in the character of the cell
    CellCharacter characterSigmas() const;
    //! Return the reciprocal character of the cell
    CellCharacter reciprocalCharacter() const;

    //! Reduce the unit cell to Niggli or conventional cell. Returns the number 
    //! according to the classification into 44 lattice types.
    int reduce(bool niggli_only, double niggliTolerance, double gruberTolerance);

    //! transform the unit cell (perform a change of basis)
    //! Note: in the future it might be better to make this method private
    void transform(const Eigen::Matrix3d& P);

    //! Check if two unit cells are equivalent, within tolerance.
    //! Equivalence means that there exist matrices T, S with integral entries such that ST = 1
    //! and A1 = A0*T, A0 = A1*S
    bool equivalent(const UnitCell& other, double tolerance) const;

    //! Return the matrix P transforming the Niggli cell to the conventional cell
    const Eigen::Matrix3d& niggliTransformation() const;

    //! Return the orientation matrix Q such that _A = Q*R where R is 
    //! upper triangular with positive entries on the diagonal, i.e. transformation
    //! mapping crystal space into real space.
    Eigen::Matrix3d orientation() const;

    //! Return the orientation matrix Q such that _A*_NP^{-1} = Q*R where R is 
    //! upper triangular with positive entries on the diagonal. This is similar to UnitCell::orientation()
    //! except that the orientation is computed for the Niggli cell.
    Eigen::Matrix3d niggliOrientation() const;

    //! \brief Return parameters of the unit cell in an internal format.
    Eigen::VectorXd parameters() const;
    //! \brief Set the uncertainty in the cell parameters.
    //! We use the parameter uncertainty and propagation of error to estimate the uncertainty in the cell parameters
    // \f$a\f$,\f$b\f$,\f$c\f$,\f$\alpha\f$,\f$\beta\f$,\f$\gamma\f$.
    void setParameterCovariance(const Eigen::MatrixXd& cov);

    //! \brief Construct a new unit cell from a reference orientation, an orientation offset, and a set of parameters.
    UnitCell fromParameters(const Eigen::Matrix3d& U0, const Eigen::Vector3d& uOffset, const Eigen::VectorXd& parameters) const;

    bool getMillerIndices(const ReciprocalVector& q, Eigen::RowVector3d& hkl, bool applyUCTolerance=true) const;

    Eigen::RowVector3i getIntegerMillerIndices(const ReciprocalVector& q) const;
    
    #ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    #endif

private:
    //! Real-space basis of column vectors
    Eigen::Matrix3d _A;
    //! Reciprocal-space basis of row vectors
    Eigen::Matrix3d _B;
    //! _NP is the transformation such that _A*_NP.inverse() is the Niggli cell
    Eigen::Matrix3d _NP; 

    sptrMaterial _material;
    LatticeCentring _centring;
    BravaisType _bravaisType;
    unsigned int _Z;
    SpaceGroup _group;
    std::string _name;
    double _hklTolerance;
    NiggliCharacter _niggli;
    CellCharacter _characterSigmas;
};

//! Print to a stream
std::ostream& operator<<(std::ostream&,const UnitCell&);

} // end namespace nsx
