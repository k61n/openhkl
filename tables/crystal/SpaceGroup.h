//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/SpaceGroup.h
//! @brief     Defines class SpaceGroup
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_TABLES_CRYSTAL_SPACEGROUP_H
#define NSX_TABLES_CRYSTAL_SPACEGROUP_H

#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/SymOp.h"

namespace ohkl {

//! Class to enumerate and construct the 3d spacegroups.

class SpaceGroup {
 public:
    //! Returns the list of spacegroup symbols
    static std::vector<std::string> symbols();

    //! Construct a space group from its IT symbol. Lookup in the IUCR table
    SpaceGroup(std::string symbol = "P 1");

    //! Returns the IT symbol for this space group
    const std::string& symbol() const;
    //! Returns the string containing the generators (in Jones notation), separated by ";"
    const std::string& generators() const;
    //! Returns a vector containing the Symmetry operations for this space group
    const SymOpList& groupElements() const;
    //! Determine whether a h,k,l reflection is forbidden
    bool isExtinct(const MillerIndex& hkl) const;
    //! Returns true if centrosymmetric
    bool isCentrosymmetric() const;
    //! Print to a stream
    void print(std::ostream& os) const;
    //! Returns the type of cell (triclinic, monoclinic ...)
    char bravaisType() const;
    //! Returns the percentage of extinct reflections
    double fractionExtinct(const MillerIndexList& hkls) const;
    //! Returns the Bravais type symbol
    std::string bravaisTypeSymbol() const;
    //! Returns the ID of the space group
    int id() const;
    //! Returns whether two sets of indices are related by a symmetry
    bool isEquivalent(const MillerIndex& hkl1, const MillerIndex& hkl2, bool friedel = false) const;
    //! Returns whether two sets of indices are related by a symmetry up to Friedel reflection
    bool isFriedelEquivalent(const MillerIndex& hkl1, const MillerIndex& hkl2) const;
    //! Return the symbol as a string
    std::string toString() const;

 private:
    void generateGroupElements();

 private:
    std::string _symbol;

    std::string _generators;

    SymOpList _groupElements;
};

std::ostream& operator<<(std::ostream& os, const SpaceGroup& sg);

} // namespace ohkl

#endif // NSX_TABLES_CRYSTAL_SPACEGROUP_H
