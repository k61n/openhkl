//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/NiggliCharacter.h
//! @brief     Defines struct NiggliCharacter
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_TABLES_CRYSTAL_NIGGLICHARACTER_H
#define NSX_TABLES_CRYSTAL_NIGGLICHARACTER_H

#include <Eigen/Dense>

namespace nsx {

//! Centering type of the Bravais lattice
enum class LatticeCentring : char { P = 'P', A = 'A', C = 'C', I = 'I', F = 'F', R = 'R' };

//! Bravais type
enum class BravaisType : char {
    Triclinic = 'a',
    Monoclinic = 'm',
    Orthorhombic = 'o',
    Tetragonal = 't',
    Hexagonal = 'h',
    Cubic = 'c',
    Trigonal = 'h'
};

// TODO refactor into LatticeCharacter
//! Helper struct used to store the classified type of the Niggli cell
struct NiggliCharacter {
    int number = 31; //!< number according to tables
    bool typeI = true; //!< true if type I, else type II
    std::string bravais = "aP"; //!< Bravais type
    Eigen::MatrixXd C; //!< matrix of linear constraints
    Eigen::Matrix3d P = Eigen::Matrix3d::Identity(); //!< transformation matrix

    bool operator==(const NiggliCharacter& other) const
    {
        return (
            (number == other.number) && (typeI == other.typeI) && (bravais == other.bravais)
            && (C == other.C) && (P == other.P));
    }

    bool operator!=(const NiggliCharacter& other) const
    {
        return (
            (number != other.number) || (typeI != other.typeI) || (bravais != other.bravais)
            || (C != other.C) || (P != other.P));
    }

    //! Initialize the condition according to priority in the table of characters
    //! with s = sign(D+E+F) and t = sign(2D+F)
    bool set(int priority, double s, double t);
};

} // namespace nsx

#endif // NSX_TABLES_CRYSTAL_NIGGLICHARACTER_H
