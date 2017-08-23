/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
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

#ifndef NSXLIB_SPACEGROUP_H
#define NSXLIB_SPACEGROUP_H

#include <array>
#include <ostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "../crystal/CrystalTypes.h"
#include "../crystal/SymOp.h"

namespace nsx {

class SpaceGroup {

public:

    static std::vector<SpaceGroupSymmetry> symmetry_table;

public:
    //! Construct a space group from its IT symbol. Lookup in the IUCR table
    SpaceGroup(std::string symbol);
    //! Copy
    SpaceGroup(const SpaceGroup& other) = default;
    //! Assignment
    SpaceGroup& operator=(const SpaceGroup& other);
    //! Get the IT symbol for this space group
    const std::string& getSymbol() const;
    //! Get the string containing the generators (in the Jones notation), separated bt ";' character
    const std::string& generators() const;
    //! Get a vector containing the Symmetry operations for this space group
    const SymOpList& getGroupElements() const;
    //! Determine whether a h,k,l reflection is forbidden
    bool isExtinct(double h, double k, double l) const;
    //! Return true if centrosymmetric
    bool isCentrosymmetric() const;
    //! Print to a stream
    void print(std::ostream& os) const;
    //! Return the type of cell (triclinic, monoclinic ...)
    char getBravaisType() const;
    //! Return the percentage of extinct reflections
    double fractionExtinct(std::vector<std::array<double, 3>> hkl);
    //! Return the Bravais type symbol
    std::string getBravaisTypeSymbol() const;
    //! Return the ID of the space group
    int getID() const;
    //! Find equivalences in a list of peaks
    std::vector<PeakList>
    findEquivalences(const PeakList& peak_list, bool friedel=true) const;
    //! Return whether two sets of indices are related by a symmetry
    bool isEquivalent(double h1, double k1, double l1, double h2, double k2, double l2, bool friedel=false) const;
    //! Return whether two sets of indices are related by a symmetry
    bool isEquivalent(const Eigen::Vector3d& a, const Eigen::Vector3d& b, bool friedel=false) const;
    //! Return whether two sets of indices are related by a symmetry up to Friedel reflection
    bool isFriedelEquivalent(double h1, double k1, double l1, double h2, double k2, double l2) const;

private:

    void generateGroupElements();

    std::string _symbol;
    std::string _generators;
    SymOpList _groupElements;
};

std::ostream& operator<<(std::ostream& os, const SpaceGroup& sg);

} // end namespace nsx

#endif // NSXLIB_SPACEGROUP_H
