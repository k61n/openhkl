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

#include <string>

#include <Eigen/Dense>

namespace nsx {
    
//! Centering type of the Bravais lattice
enum  class LatticeCentring : char  {P='P',A='A',C='C',I='I',F='F',R='R'};
//! Bravais type
enum  class BravaisType : char  {Triclinic='a',Monoclinic='m',Orthorhombic='o',Tetragonal='t',Hexagonal='h',Cubic='c',Trigonal='h'};


// todo: refactor into LatticeCharacter
//! Structure to handle a Niggli character
struct NiggliCharacter {
    int number = 31; // number according to tables
    bool typeI = true; // true if type I, else type II
    std::string bravais = "aP"; // Bravais type
    Eigen::MatrixXd C; // matrix of linear constraints
    Eigen::Matrix3d P = Eigen::Matrix3d::Identity(); // transformation matrix
    
    // initialize the condition according to priority
    // s = sign(D+E+F)
    // t = sign(2D+F)
    bool set(int priority, double s, double t);
};

} // end namespace nsx
