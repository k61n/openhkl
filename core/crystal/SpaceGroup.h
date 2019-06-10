//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/crystal/SpaceGroup.h
//! @brief     Defines class SpaceGroup
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_CRYSTAL_SPACEGROUP_H
#define CORE_CRYSTAL_SPACEGROUP_H

#include "core/crystal/MillerIndex.h"
#include "core/crystal/SymOp.h"

namespace nsx {

//! \class SpaceGroup
//! Class to enumerate and construct the 3d spacegroups.
class SpaceGroup {
private:
    //! Returns the list of spacegroup symmetry generators associated to each
    //! spacegroup
    static const std::vector<SpaceGroupSymmetry> symmetry_table;

public:
    //! Returns the list of spacegroup symbols
    static std::vector<std::string> symbols();
    SpaceGroup();
    //! Construct a space group from its IT symbol. Lookup in the IUCR table
    SpaceGroup(std::string symbol);
    //! Copy
    SpaceGroup(const SpaceGroup& other) = default;
    //! Assignment
    SpaceGroup& operator=(const SpaceGroup& other);
    //! Gets the IT symbol for this space group
    const std::string& symbol() const;
    //! Gets the string containing the generators (in the Jones notation),
    //! separated bt ";' character
    const std::string& generators() const;
    //! Gets a vector containing the Symmetry operations for this space group
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
    bool isEquivalent(
        const MillerIndex& miller_index1, const MillerIndex& miller_index2,
        bool friedel = false) const;
    //! Returns whether two sets of indices are related by a symmetry up to Friedel
    //! reflection
    bool
    isFriedelEquivalent(const MillerIndex& miller_index1, const MillerIndex& miller_index2) const;
    //! Sort peak into a list of equivalent peaks, using the space group symmetry,
    //! optinally including Friedel pairs (if this is not already a symmetry of
    //! the space group)
    std::vector<PeakList> findEquivalences(const PeakList& peak_list, bool friedel) const;

private:
    void reduceSymbol();

    void generateGroupElements();

private:
    std::string _symbol;

    std::string _generators;

    SymOpList _groupElements;
};

std::ostream& operator<<(std::ostream& os, const SpaceGroup& sg);

} // namespace nsx

#endif // CORE_CRYSTAL_SPACEGROUP_H
