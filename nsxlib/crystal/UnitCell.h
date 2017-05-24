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

#ifndef NSXLIB_UNITCELL_H
#define NSXLIB_UNITCELL_H

#include <vector>
#include <string>

#include <Eigen/Dense>

#include "../chemistry/ChemistryTypes.h"
#include "../crystal/SpaceGroup.h"
#include "../geometry/Basis.h"

namespace nsx {

//! Centering type of the Bravais lattice
enum  class LatticeCentring : char  {P='P',A='A',B='B',C='C',I='I',F='F',R='R'};
//! Bravais type
enum  class BravaisType : char  {Triclinic='a',Monoclinic='m',Orthorhombic='o',Tetragonal='t',Hexagonal='h',Cubic='c',Trigonal='h'};

/** @brief Class to define a crystallographic unit-cell.
 *
 * Provide functionalities to transform vectors of the direct lattice or reciprocal
 * lattice from unit-cell coordinates to a right-handed coordinates system.
 * The UnitCell is defined by the parameters a,b,c and angles alpha, beta, gamma.
 * The UnitCell parameters a,b,c are given in \f$ \AA \f$  and angle alpha, beta, gamma
 * are given in degrees.
 * The a axis is chosen as pointing along the x-direction, and the b-axis is in the xy-plane.
 */
class UnitCell : public Basis
{
public:

    //! Empty UnitCell, initialiazed to right-handed orthonormal system
    UnitCell();
    //! Construct unitCell from lattice parameters, the A matrix is built with avector along x, bvector in the xy-plane.
    UnitCell(double a, double b, double c, double alpha, double beta, double gamma,LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic, std::shared_ptr<Basis> reference=nullptr);
    //! Build Unit-cell from 3 non-coplanar vectors. Throw if coplanars.
    UnitCell(const Eigen::Vector3d& v1,const Eigen::Vector3d& v2,const Eigen::Vector3d& v3, LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic,std::shared_ptr<Basis> reference=nullptr );
    //! Copy constructor
    UnitCell(const UnitCell&);
    //! Assignment
    UnitCell& operator=(const UnitCell& other);
    ~UnitCell();
    //! Set lattice parameters
    void setParams(double a, double b, double c, double alpha, double beta, double gamma);

    //! Set lattice parmeters from ABCDEF
    void setABCDEF(double A, double B, double C, double D, double E, double F);

    //! Get A vector
    Eigen::Vector3d getAVector() const;
    //! Get B vector
    Eigen::Vector3d getBVector() const;
    //! Get C vector
    Eigen::Vector3d getCVector() const;
    //! Get A* vector
    Eigen::Vector3d getReciprocalAVector() const;
    //! Get B* vector
    Eigen::Vector3d getReciprocalBVector() const;
    //! Get C* vector
    Eigen::Vector3d getReciprocalCVector() const;

    void setLatticeVectors(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c);
    void copyMatrices(const UnitCell& other);
    //! Build a UnitCell from a set of three direct vectors.
    static UnitCell fromDirectVectors(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c, LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic,const std::shared_ptr<Basis>& reference=nullptr);
    //! Build a UnitCell from a set of three reciprocal vectors.
    static UnitCell fromReciprocalVectors(const Eigen::Vector3d& a, const Eigen::Vector3d& b, const Eigen::Vector3d& c,LatticeCentring centring=LatticeCentring::P, BravaisType bravais=BravaisType::Triclinic, const std::shared_ptr<Basis>& reference=nullptr);
    //!
    void setLatticeCentring(LatticeCentring centring);
    //!
    void setBravaisType(BravaisType bravais);
    //!
    std::string getBravaisTypeSymbol() const;
    //! Get a list of reflections sorted by their d* value in $\AA$ within a sphere defined by dstarmax
    std::vector<Eigen::Vector3d> generateReflectionsInSphere(double dstarmax) const;
    //! Get a list of reflections with d value in the range [dmin, dmax]
    std::vector<Eigen::Vector3d> generateReflectionsInShell(double dmin, double dmax, double wavelength) const;
    //! Return the angle in radians between two reflections h1,k1,l1 and h2,k2,l2
    double getAngle(double h1,double k1, double l1, double h2, double k2, double l2) const;
    //! Return the angle in radians between two reflections hkl1 and hkl2
    double getAngle(const Eigen::Vector3d& hkl1,const Eigen::Vector3d& hkl2) const;
    //! Get UB
    //! Determine whether two reflections (h1,k1,l1) and (h2,k2,l2) are equivalent.
    bool isEquivalent(double h1,double k1, double l1, double h2, double k2, double l2) const;
    //! Determine whether two reflections (h1,k1,l1) and (h2,k2,l2) are equivalent up to Friedel reflection.
    bool isFriedelEquivalent(double h1,double k1, double l1, double h2, double k2, double l2) const;

    void setBU(const Eigen::Vector3d& hkl1,const Eigen::Vector3d& hkl2, const Eigen::Vector3d& q1, const Eigen::Vector3d& q2);
    void setBU(const Eigen::Matrix3d& BU);


    //! Return d of the Bragg condition for the given reflection
    double getD(int h, int k, int l);

    //! Get the Busing-Levy B matrix as defined in Acta Cryst. (1967). 22, 457
    //! The returned matrix is the transposed version of the matrix since
    //! reciprocal bases are contravariant in NSXTool.
    Eigen::Matrix3d getBusingLevyB() const;
    Eigen::Matrix3d getBusingLevyU() const;

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
    //! Set space group from its symbol
    void setSpaceGroup(const std::string& symbol);
    std::string getSpaceGroup() const;

    //! Set name of the unit cell
    void setName(const std::string& name);
    //! Get name of the unit cell
    const std::string& getName() const;

    //! Set the integer tolerance for this unit cell
    void setHKLTolerance(double tolerance);
    //! Get the integer tolerance for this unit cell
    double getHKLTolerance() const;

private:
    sptrMaterial _material;
    LatticeCentring _centring;
    BravaisType _bravaisType;
    unsigned int _Z;
    SpaceGroup _group;
    std::string _name;
    double _hklTolerance;
};

//! Print to a stream
std::ostream& operator<<(std::ostream&,const UnitCell&);

} // end namespace nsx

#endif // NSXLIB_UNITCELL_H
