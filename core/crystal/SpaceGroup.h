#pragma once

#include <array>
#include <ostream>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "MillerIndex.h"
#include "SymOp.h"

namespace nsx {

//! \class SpaceGroup
//! \brief Class to enumerate and construct the 3d spacegroups.
class SpaceGroup {
private:
    //! Return the list of spacegroup symmetry generators associated to each
    //! spacegroup
    static const std::vector<SpaceGroupSymmetry> symmetry_table;

public:
    //! Return the list of spacegroup symbols
    static std::vector<std::string> symbols();
    SpaceGroup();
    //! Construct a space group from its IT symbol. Lookup in the IUCR table
    SpaceGroup(std::string symbol);
    //! Copy
    SpaceGroup(const SpaceGroup& other) = default;
    //! Assignment
    SpaceGroup& operator=(const SpaceGroup& other);
    //! Get the IT symbol for this space group
    const std::string& symbol() const;
    //! Get the string containing the generators (in the Jones notation),
    //! separated bt ";' character
    const std::string& generators() const;
    //! Get a vector containing the Symmetry operations for this space group
    const SymOpList& groupElements() const;
    //! Determine whether a h,k,l reflection is forbidden
    bool isExtinct(const MillerIndex& hkl) const;
    //! Return true if centrosymmetric
    bool isCentrosymmetric() const;
    //! Print to a stream
    void print(std::ostream& os) const;
    //! Return the type of cell (triclinic, monoclinic ...)
    char bravaisType() const;
    //! Return the percentage of extinct reflections
    double fractionExtinct(const MillerIndexList& hkls) const;
    //! Return the Bravais type symbol
    std::string bravaisTypeSymbol() const;
    //! Return the ID of the space group
    int id() const;
    //! Return whether two sets of indices are related by a symmetry
    bool isEquivalent(
        const MillerIndex& miller_index1, const MillerIndex& miller_index2,
        bool friedel = false) const;
    //! Return whether two sets of indices are related by a symmetry up to Friedel
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

} // end namespace nsx
