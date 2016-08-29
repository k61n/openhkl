#include <sstream>

#include <boost/algorithm/string/join.hpp>

#include "Error.h"
#include "SpaceGroupSymbols.h"

namespace SX
{
namespace Crystal
{
/* Copied from the CRYSFML library, module CFML_Symmetry_Tables
!!-------------------------------------------------------
!!---- Crystallographic Fortran Modules Library (CrysFML)
!!-------------------------------------------------------
!!---- The CrysFML project is distributed under LGPL. In agreement with the
!!---- Intergovernmental Convention of the ILL, this software cannot be used
!!---- in military applications.
!!----
!!---- Copyright (C) 1999-2012  Institut Laue-Langevin (ILL), Grenoble, FRANCE
!!----                          Universidad de La Laguna (ULL), Tenerife, SPAIN
!!----                          Laboratoire Leon Brillouin(LLB), Saclay, FRANCE
!!----
!!---- Authors: Juan Rodriguez-Carvajal (ILL)
!!----          Javier Gonzalez-Platas  (ULL)
!!----
!!---- Contributors: Laurent Chapon     (ILL)
!!----               Marc Janoschek     (Los Alamos National Laboratory, USA)
!!----               Oksana Zaharko     (Paul Scherrer Institute, Switzerland)
!!----               Tierry Roisnel     (CDIFX,Rennes France)
!!----               Eric Pellegrini    (ILL)
!!----
!!---- This library is free software; you can redistribute it and/or
!!---- modify it under the terms of the GNU Lesser General Public
!!---- License as published by the Free Software Foundation; either
!!---- version 3.0 of the License, or (at your option) any later version.
!!----
!!---- This library is distributed in the hope that it will be useful,
!!---- but WITHOUT ANY WARRANTY; without even the implied warranty of
!!---- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
!!---- Lesser General Public License for more details.
!!----
!!---- You should have received a copy of the GNU Lesser General Public
!!---- License along with this library; if not, see <http://www.gnu.org/licenses/>.
!!----
!!----
!!---- MODULE: CFML_Symmetry_Tables
!!----   INFO: Tabulated information on Crystallographic Symmetry
*/

std::unordered_map<std::string,std::string> SpaceGroupSymbols::_spaceGroupTables =
{
	           {"P 1"," x,y,z"},
		       {"P -1"," -x,-y,-z"},
		       {"P 2"," -x,y,-z"},
		       {"P 21"," -x,y+1/2,-z"},
		       {"C 2"," x+1/2,y+1/2,z; -x,y,-z"},
		       {"P m"," x,-y,z"},
		       {"P c"," x,-y,z+1/2"},
		       {"C m"," x+1/2,y+1/2,z; x,-y,z"},
		       {"C c"," x+1/2,y+1/2,z; x,-y,z+1/2"},
		       {"P 2/m"," -x,y,-z; -x,-y,-z"},
		       {"P 21/m"," -x,y+1/2,-z; -x,-y,-z"},
		       {"C 2/m"," x+1/2,y+1/2,z; -x,y,-z; -x,-y,-z"},
		       {"P 2/c"," -x,y,-z+1/2; -x,-y,-z"},
		       {"P 21/c"," -x,y+1/2,-z+1/2; -x,-y,-z"},
		       {"C 2/c"," x+1/2,y+1/2,z; -x,y,-z+1/2; -x,-y,-z"},
		       {"P 2 2 2"," -x,-y,z; -x,y,-z"},
		       {"P 2 2 21"," -x,-y,z+1/2; -x,y,-z+1/2"},
		       {"P 21 21 2"," -x,-y,z; -x+1/2,y+1/2,-z"},
		       {"P 21 21 21"," -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2"},
		       {"C 2 2 21"," x+1/2,y+1/2,z; -x,-y,z+1/2; -x,y,-z+1/2"},
		       {"C 2 2 2"," x+1/2,y+1/2,z; -x,-y,z; -x,y,-z"},
		       {"F 2 2 2"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z"},
		       {"I 2 2 2"," x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z"},
		       {"I 21 21 21"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2"},
		       {"P m m 2"," -x,-y,z; x,-y,z"},
		       {"P m c 21"," -x,-y,z+1/2; x,-y,z+1/2"},
		       {"P c c 2"," -x,-y,z; x,-y,z+1/2"},
		       {"P m a 2"," -x,-y,z; x+1/2,-y,z"},
		       {"P c a 21"," -x,-y,z+1/2; x+1/2,-y,z"},
		       {"P n c 2"," -x,-y,z; x,-y+1/2,z+1/2"},
		       {"P m n 21"," -x+1/2,-y,z+1/2; x+1/2,-y,z+1/2"},
		       {"P b a 2"," -x,-y,z; x+1/2,-y+1/2,z"},
		       {"P n a 21"," -x,-y,z+1/2; x+1/2,-y+1/2,z"},
		       {"P n n 2"," -x,-y,z; x+1/2,-y+1/2,z+1/2"},
		       {"C m m 2"," x+1/2,y+1/2,z; -x,-y,z; x,-y,z"},
		       {"C m c 21"," x+1/2,y+1/2,z; -x,-y,z+1/2; x,-y,z+1/2"},
		       {"C c c 2"," x+1/2,y+1/2,z; -x,-y,z; x,-y,z+1/2"},
		       {"A m m 2"," x,y+1/2,z+1/2; -x,-y,z; x,-y,z"},
		       {"A b m 2"," x,y+1/2,z+1/2; -x,-y,z; x,-y+1/2,z"},
		       {"A m a 2"," x,y+1/2,z+1/2; -x,-y,z; x+1/2,-y,z"},
		       {"A b a 2"," x,y+1/2,z+1/2; -x,-y,z; x+1/2,-y+1/2,z"},
		       {"F m m 2"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; x,-y,z"},
		       {"F d d 2"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; x+1/4,-y+1/4,z+1/4"},
		       {"I m m 2"," x+1/2,y+1/2,z+1/2; -x,-y,z; x,-y,z"},
		       {"I b a 2"," x+1/2,y+1/2,z+1/2; -x,-y,z; x+1/2,-y+1/2,z"},
		       {"I m a 2"," x+1/2,y+1/2,z+1/2; -x,-y,z; x+1/2,-y,z"},
		       {"P m m m"," -x,-y,z; -x,y,-z; -x,-y,-z"},
		       {"P n n n"," -x+1/2,-y+1/2,z; -x+1/2,y,-z+1/2; -x,-y,-z"},
		       {"P c c m"," -x,-y,z; -x,y,-z+1/2; -x,-y,-z"},
		       {"P b a n"," -x+1/2,-y+1/2,z; -x+1/2,y,-z; -x,-y,-z"},
		       {"P m m a"," -x+1/2,-y,z; -x,y,-z; -x,-y,-z"},
		       {"P n n a"," -x+1/2,-y,z; -x+1/2,y+1/2,-z+1/2; -x,-y,-z"},
		       {"P m n a"," -x+1/2,-y,z+1/2; -x+1/2,y,-z+1/2; -x,-y,-z"},
		       {"P c c a"," -x+1/2,-y,z; -x,y,-z+1/2; -x,-y,-z"},
		       {"P b a m"," -x,-y,z; -x+1/2,y+1/2,-z; -x,-y,-z"},
		       {"P c c n"," -x+1/2,-y+1/2,z; -x,y+1/2,-z+1/2; -x,-y,-z"},
		       {"P b c m"," -x,-y,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
		       {"P n n m"," -x,-y,z; -x+1/2,y+1/2,-z+1/2; -x,-y,-z"},
		       {"P m m n"," -x+1/2,-y+1/2,z; -x,y+1/2,-z; -x,-y,-z"},
		       {"P b c n"," -x+1/2,-y+1/2,z+1/2; -x,y,-z+1/2; -x,-y,-z"},
		       {"P b c a"," -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
		       {"P n m a"," -x+1/2,-y,z+1/2; -x,y+1/2,-z; -x,-y,-z"},
		       {"C m c m"," x+1/2,y+1/2,z; -x,-y,z+1/2; -x,y,-z+1/2; -x,-y,-z"},
		       {"C m c a"," x+1/2,y+1/2,z; -x,-y+1/2,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
		       {"C m m m"," x+1/2,y+1/2,z; -x,-y,z; -x,y,-z; -x,-y,-z"},
		       {"C c c m"," x+1/2,y+1/2,z; -x,-y,z; -x,y,-z+1/2; -x,-y,-z"},
		       {"C m m a"," x+1/2,y+1/2,z; -x,-y+1/2,z; -x,y+1/2,-z; -x,-y,-z"},
		       {"C c c a"," x+1/2,y+1/2,z; -x+1/2,-y,z; -x,y,-z+1/2; -x,-y,-z"},
		       {"F m m m"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; -x,-y,-z"},
		       {"F d d d"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x+3/4,-y+3/4,z; -x+3/4,y,-z+3/4; -x,-y,-z"},
		       {"I m m m"," x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; -x,-y,-z"},
		       {"I b a m"," x+1/2,y+1/2,z+1/2; -x,-y,z; -x+1/2,y+1/2,-z; -x,-y,-z"},
		       {"I b c a"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
		       {"I m m a"," x+1/2,y+1/2,z+1/2; -x,-y+1/2,z; -x,y+1/2,-z; -x,-y,-z"},
		       {"P 4"," -x,-y,z; -y,x,z"},
		       {"P 41"," -x,-y,z+1/2; -y,x,z+1/4"},
		       {"P 42"," -x,-y,z; -y,x,z+1/2"},
		       {"P 43"," -x,-y,z+1/2; -y,x,z+3/4"},
		       {"I 4"," x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z"},
		       {"I 41"," x+1/2,y+1/2,z+1/2; -x+1/2,-y+1/2,z+1/2; -y,x+1/2,z+1/4"},
		       {"P -4"," -x,-y,z; y,-x,-z"},
		       {"I -4"," x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z"},
		       {"P 4/m"," -x,-y,z; -y,x,z; -x,-y,-z"},
		       {"P 42/m"," -x,-y,z; -y,x,z+1/2; -x,-y,-z"},
		       {"P 4/n"," -x+1/2,-y+1/2,z; -y+1/2,x,z; -x,-y,-z"},
		       {"P 42/n"," -x+1/2,-y+1/2,z; -y,x+1/2,z+1/2; -x,-y,-z"},
		       {"I 4/m"," x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; -x,-y,-z"},
		       {"I 41/a"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -y+3/4,x+1/4,z+1/4; -x,-y,-z"},
		       {"P 4 2 2"," -x,-y,z; -y,x,z; -x,y,-z"},
		       {"P 4 21 2"," -x,-y,z; -y+1/2,x+1/2,z; -x+1/2,y+1/2,-z"},
		       {"P 41 2 2"," -x,-y,z+1/2; -y,x,z+1/4; -x,y,-z"},
		       {"P 41 21 2"," -x,-y,z+1/2; -y+1/2,x+1/2,z+1/4; -x+1/2,y+1/2,-z+1/4"},
		       {"P 42 2 2"," -x,-y,z; -y,x,z+1/2; -x,y,-z"},
		       {"P 42 21 2"," -x,-y,z; -y+1/2,x+1/2,z+1/2; -x+1/2,y+1/2,-z+1/2"},
		       {"P 43 2 2"," -x,-y,z+1/2; -y,x,z+3/4; -x,y,-z"},
		       {"P 43 21 2"," -x,-y,z+1/2; -y+1/2,x+1/2,z+3/4; -x+1/2,y+1/2,-z+3/4"},
		       {"I 4 2 2"," x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; -x,y,-z"},
		       {"I 41 2 2"," x+1/2,y+1/2,z+1/2; -x+1/2,-y+1/2,z+1/2; -y,x+1/2,z+1/4; -x+1/2,y,-z+3/4"},
		       {"P 4 m m"," -x,-y,z; -y,x,z; x,-y,z"},
		       {"P 4 b m"," -x,-y,z; -y,x,z; x+1/2,-y+1/2,z"},
		       {"P 42 c m"," -x,-y,z; -y,x,z+1/2; x,-y,z+1/2"},
		       {"P 42 n m"," -x,-y,z; -y+1/2,x+1/2,z+1/2; x+1/2,-y+1/2,z+1/2"},
		       {"P 4 c c"," -x,-y,z; -y,x,z; x,-y,z+1/2"},
		       {"P 4 n c"," -x,-y,z; -y,x,z; x+1/2,-y+1/2,z+1/2"},
		       {"P 42 m c"," -x,-y,z; -y,x,z+1/2; x,-y,z"},
		       {"P 42 b c"," -x,-y,z; -y,x,z+1/2; x+1/2,-y+1/2,z"},
		       {"I 4 m m"," x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; x,-y,z"},
		       {"I 4 c m"," x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; x,-y,z+1/2"},
		       {"I 41 m d"," x+1/2,y+1/2,z+1/2; -x+1/2,-y+1/2,z+1/2; -y,x+1/2,z+1/4; x,-y,z"},
		       {"I 41 c d"," x+1/2,y+1/2,z+1/2; -x+1/2,-y+1/2,z+1/2; -y,x+1/2,z+1/4; x,-y,z+1/2"},
		       {"P -4 2 m"," -x,-y,z; y,-x,-z; -x,y,-z"},
		       {"P -4 2 c"," -x,-y,z; y,-x,-z; -x,y,-z+1/2"},
		       {"P -4 21 m"," -x,-y,z; y,-x,-z; -x+1/2,y+1/2,-z"},
		       {"P -4 21 c"," -x,-y,z; y,-x,-z; -x+1/2,y+1/2,-z+1/2"},
		       {"P -4 m 2"," -x,-y,z; y,-x,-z; x,-y,z"},
		       {"P -4 c 2"," -x,-y,z; y,-x,-z; x,-y,z+1/2"},
		       {"P -4 b 2"," -x,-y,z; y,-x,-z; x+1/2,-y+1/2,z"},
		       {"P -4 n 2"," -x,-y,z; y,-x,-z; x+1/2,-y+1/2,z+1/2"},
		       {"I -4 m 2"," x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z; x,-y,z"},
		       {"I -4 c 2"," x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z; x,-y,z+1/2"},
		       {"I -4 2 m"," x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z; -x,y,-z"},
		       {"I -4 2 d"," x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z; -x+1/2,y,-z+3/4"},
		       {"P 4/m m m"," -x,-y,z; -y,x,z; -x,y,-z; -x,-y,-z"},
		       {"P 4/m c c"," -x,-y,z; -y,x,z; -x,y,-z+1/2; -x,-y,-z"},
		       {"P 4/n b m"," -x+1/2,-y+1/2,z; -y+1/2,x,z; -x+1/2,y,-z; -x,-y,-z"},
		       {"P 4/n n c"," -x+1/2,-y+1/2,z; -y+1/2,x,z; -x+1/2,y,-z+1/2; -x,-y,-z"},
		       {"P 4/m b m"," -x,-y,z; -y,x,z; -x+1/2,y+1/2,-z; -x,-y,-z"},
		       {"P 4/m n c"," -x,-y,z; -y,x,z; -x+1/2,y+1/2,-z+1/2; -x,-y,-z"},
		       {"P 4/n m m"," -x+1/2,-y+1/2,z; -y+1/2,x,z; -x,y+1/2,-z; -x,-y,-z"},
		       {"P 4/n c c"," -x+1/2,-y+1/2,z; -y+1/2,x,z; -x,y+1/2,-z+1/2; -x,-y,-z"},
		       {"P 42/m m c"," -x,-y,z; -y,x,z+1/2; -x,y,-z; -x,-y,-z"},
		       {"P 42/m c m"," -x,-y,z; -y,x,z+1/2; -x,y,-z+1/2; -x,-y,-z"},
		       {"P 42/n b c"," -x+1/2,-y+1/2,z; -y+1/2,x,z+1/2; -x+1/2,y,-z; -x,-y,-z"},
		       {"P 42/n n m"," -x+1/2,-y+1/2,z; -y+1/2,x,z+1/2; -x+1/2,y,-z+1/2; -x,-y,-z"},
		       {"P 42/m b c"," -x,-y,z; -y,x,z+1/2; -x+1/2,y+1/2,-z; -x,-y,-z"},
		       {"P 42/m n m"," -x,-y,z; -y+1/2,x+1/2,z+1/2; -x+1/2,y+1/2,-z+1/2; -x,-y,-z"},
		       {"P 42/n m c"," -x+1/2,-y+1/2,z; -y+1/2,x,z+1/2; -x,y+1/2,-z; -x,-y,-z"},
		       {"P 42/n c m"," -x+1/2,-y+1/2,z; -y+1/2,x,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
		       {"I 4/m m m"," x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; -x,y,-z; -x,-y,-z"},
		       {"I 4/m c m"," x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; -x,y,-z+1/2; -x,-y,-z"},
		       {"I 41/a m d"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -y+1/4,x+3/4,z+1/4; -x+1/2,y,-z+1/2; -x,-y,-z"},
		       {"I 41/a c d"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -y+1/4,x+3/4,z+1/4; -x+1/2,y,-z; -x,-y,-z"},
		       {"P 3"," -y,x-y,z"},
		       {"P 31"," -y,x-y,z+1/3"},
		       {"P 32"," -y,x-y,z+2/3"},
		       {"R 3"," x+1/3,y+2/3,z+2/3; -y,x-y,z"},
		       {"P -3"," -y,x-y,z; -x,-y,-z"},
		       {"R -3"," x+1/3,y+2/3,z+2/3; -y,x-y,z; -x,-y,-z"},
		       {"P 3 1 2"," -y,x-y,z; -y,-x,-z"},
		       {"P 3 2 1"," -y,x-y,z; y,x,-z"},
		       {"P 31 1 2"," -y,x-y,z+1/3; -y,-x,-z+2/3"},
		       {"P 31 2 1"," -y,x-y,z+1/3; y,x,-z"},
		       {"P 32 1 2"," -y,x-y,z+2/3; -y,-x,-z+1/3"},
		       {"P 32 2 1"," -y,x-y,z+2/3; y,x,-z"},
		       {"R 3 2"," x+1/3,y+2/3,z+2/3; -y,x-y,z; y,x,-z"},
		       {"P 3 m 1"," -y,x-y,z; -y,-x,z"},
		       {"P 3 1 m"," -y,x-y,z; y,x,z"},
		       {"P 3 c 1"," -y,x-y,z; -y,-x,z+1/2"},
		       {"P 3 1 c"," -y,x-y,z; y,x,z+1/2"},
		       {"R 3 m"," x+1/3,y+2/3,z+2/3; -y,x-y,z; -y,-x,z"},
		       {"R 3 c"," x+1/3,y+2/3,z+2/3; -y,x-y,z; -y,-x,z+1/2"},
		       {"P -3 1 m"," -y,x-y,z; -y,-x,-z; -x,-y,-z"},
		       {"P -3 1 c"," -y,x-y,z; -y,-x,-z+1/2; -x,-y,-z"},
		       {"P -3 m 1"," -y,x-y,z; y,x,-z; -x,-y,-z"},
		       {"P -3 c 1"," -y,x-y,z; y,x,-z+1/2; -x,-y,-z"},
		       {"R -3 m"," x+1/3,y+2/3,z+2/3; -y,x-y,z; y,x,-z; -x,-y,-z"},
		       {"R -3 c"," x+1/3,y+2/3,z+2/3; -y,x-y,z; y,x,-z+1/2; -x,-y,-z"},
		       {"P 6"," -y,x-y,z; -x,-y,z"},
		       {"P 61"," -y,x-y,z+1/3; -x,-y,z+1/2"},
		       {"P 65"," -y,x-y,z+2/3; -x,-y,z+1/2"},
		       {"P 62"," -y,x-y,z+2/3; -x,-y,z"},
		       {"P 64"," -y,x-y,z+1/3; -x,-y,z"},
		       {"P 63"," -y,x-y,z; -x,-y,z+1/2"},
		       {"P -6"," -y,x-y,z; x,y,-z"},
		       {"P 6/m"," -y,x-y,z; -x,-y,z; -x,-y,-z"},
		       {"P 63/m"," -y,x-y,z; -x,-y,z+1/2; -x,-y,-z"},
		       {"P 6 2 2"," -y,x-y,z; -x,-y,z; y,x,-z"},
		       {"P 61 2 2"," -y,x-y,z+1/3; -x,-y,z+1/2; y,x,-z+1/3"},
		       {"P 65 2 2"," -y,x-y,z+2/3; -x,-y,z+1/2; y,x,-z+2/3"},
		       {"P 62 2 2"," -y,x-y,z+2/3; -x,-y,z; y,x,-z+2/3"},
		       {"P 64 2 2"," -y,x-y,z+1/3; -x,-y,z; y,x,-z+1/3"},
		       {"P 63 2 2"," -y,x-y,z; -x,-y,z+1/2; y,x,-z"},
		       {"P 6 m m"," -y,x-y,z; -x,-y,z; -y,-x,z"},
		       {"P 6 c c"," -y,x-y,z; -x,-y,z; -y,-x,z+1/2"},
		       {"P 63 c m"," -y,x-y,z; -x,-y,z+1/2; -y,-x,z+1/2"},
		       {"P 63 m c"," -y,x-y,z; -x,-y,z+1/2; -y,-x,z"},
		       {"P -6 m 2"," -y,x-y,z; x,y,-z; -y,-x,z"},
		       {"P -6 c 2"," -y,x-y,z; x,y,-z+1/2; -y,-x,z+1/2"},
		       {"P -6 2 m"," -y,x-y,z; x,y,-z; y,x,-z"},
		       {"P -6 2 c"," -y,x-y,z; x,y,-z+1/2; y,x,-z"},
		       {"P 6/m m m"," -y,x-y,z; -x,-y,z; y,x,-z; -x,-y,-z"},
		       {"P 6/m c c"," -y,x-y,z; -x,-y,z; y,x,-z+1/2; -x,-y,-z"},
		       {"P 63/m c m"," -y,x-y,z; -x,-y,z+1/2; y,x,-z+1/2; -x,-y,-z"},
		       {"P 63/m m c"," -y,x-y,z; -x,-y,z+1/2; y,x,-z; -x,-y,-z"},
		       {"P 2 3"," -x,-y,z; -x,y,-z; z,x,y"},
		       {"F 2 3"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y"},
		       {"I 2 3"," x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y"},
		       {"P 21 3"," -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y"},
		       {"I 21 3"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y"},
		       {"P m -3"," -x,-y,z; -x,y,-z; z,x,y; -x,-y,-z"},
		       {"P n -3"," -x+1/2,-y+1/2,z; -x+1/2,y,-z+1/2; z,x,y; -x,-y,-z"},
		       {"F m -3"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; -x,-y,-z"},
		       {"F d -3"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x+1/4,-y+1/4,z; -x+1/4,y,-z+1/4; z,x,y; -x,-y,-z"},
		       {"I m -3"," x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y; -x,-y,-z"},
		       {"P a -3"," -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; -x,-y,-z"},
		       {"I a -3"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; -x,-y,-z"},
		       {"P 4 3 2"," -x,-y,z; -x,y,-z; z,x,y; y,x,-z"},
		       {"P 42 3 2"," -x,-y,z; -x,y,-z; z,x,y; y+1/2,x+1/2,-z+1/2"},
		       {"F 4 3 2"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,-z"},
		       {"F 41 3 2"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y+1/2,z+1/2; -x+1/2,y+1/2,-z; z,x,y; y+3/4,x+1/4,-z+3/4"},
		       {"I 4 3 2"," x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,-z"},
		       {"P 43 3 2"," -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; y+1/4,x+3/4,-z+3/4"},
		       {"P 41 3 2"," -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; y+3/4,x+1/4,-z+1/4"},
		       {"I 41 3 2"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; y+3/4,x+1/4,-z+1/4"},
		       {"P -4 3 m"," -x,-y,z; -x,y,-z; z,x,y; y,x,z"},
		       {"F -4 3 m"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,z"},
		       {"I -4 3 m"," x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,z"},
		       {"P -4 3 n"," -x,-y,z; -x,y,-z; z,x,y; y+1/2,x+1/2,z+1/2"},
		       {"F -4 3 c"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y+1/2,x+1/2,z+1/2"},
		       {"I -4 3 d"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; y+1/4,x+1/4,z+1/4"},
		       {"P m -3 m"," -x,-y,z; -x,y,-z; z,x,y; y,x,-z; -x,-y,-z"},
		       {"P n -3 n"," -x+1/2,-y+1/2,z; -x+1/2,y,-z+1/2; z,x,y; y,x,-z+1/2; -x,-y,-z"},
		       {"P m -3 n"," -x,-y,z; -x,y,-z; z,x,y; y+1/2,x+1/2,-z+1/2; -x,-y,-z"},
		       {"P n -3 m"," -x+1/2,-y+1/2,z; -x+1/2,y,-z+1/2; z,x,y; y+1/2,x+1/2,-z; -x,-y,-z"},
		       {"F m -3 m"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,-z; -x,-y,-z"},
		       {"F m -3 c"," x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y+1/2,x+1/2,-z+1/2; -x,-y,-z"},
               {"F d -3 m"," x+1/2,y+1/2,z; x+1/2,y,z+1/2;-x+3/4,-y+1/4,z+1/2;-x+1/4,y+1/2,-z+3/4;z,x,y;y+3/4,x+1/4,-z+1/2;-x,-y,-z"},
		       {"F d -3 c"," x+1/2,y+1/2,z; x+1/2,y,z+1/2;-x+1/4,-y+3/4,z+1/2;-x+3/4,y+1/2,-z+1/4;z,x,y;y+3/4,x+1/4,-z;-x,-y,-z"},
		       {"I m -3 m"," x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,-z; -x,-y,-z"},
		       {"I a -3 d"," x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; y+3/4,x+1/4,-z+1/4; -x,-y,-z"}
};

void SpaceGroupSymbols::addSpaceGroup(const std::string& spaceGroup, const std::string& generators)
{
	auto it=_spaceGroupTables.find(spaceGroup);
	if (it!=_spaceGroupTables.end())
		throw SX::Kernel::Error<SpaceGroupSymbols>("The space group "+spaceGroup+" is already registered.");
	_spaceGroupTables.insert(std::unordered_map<std::string,std::string>::value_type(spaceGroup,generators));
}

bool SpaceGroupSymbols::getGenerators(const std::string& spacegroup, std::string& generators)
{
	auto it=_spaceGroupTables.find(spacegroup);
	if (it==_spaceGroupTables.end())
		return false;
	generators=(*it).second;
	return true;
}

std::string SpaceGroupSymbols::getReducedSymbol(const std::string& symbol) const
{
	// This is the only get when the separate 1 has to be kept
    if (symbol == "P 1")
		return symbol;
    else if (symbol == "P 3 1 m")
        return symbol;
    else if (symbol == "P 3 m 1")
        return symbol;
    else if (symbol == "P -3 1 m")
        return symbol;
    else if (symbol == "P -3 m 1")
        return symbol;
    else if (symbol == "P 3 1 c")
        return symbol;
    else if (symbol == "P 3 c 1")
        return symbol;
    else if (symbol == "P -3 1 c")
        return symbol;
    else if (symbol == "P -3 c 1")
        return symbol;
    else if (symbol == "P 32 2 1")
        return symbol;
    else if (symbol == "P 32 1 2")
        return symbol;
    else if (symbol == "P 31 1 2")
        return symbol;
    else if (symbol == "P 31 2 1")
        return symbol;
    else if (symbol == "P 3 1 2")
        return symbol;
    else if (symbol == "P 3 2 1")
        return symbol;

	// Otherwise throw away every separate 1 to produce the short name for Bravais
	// see https://en.wikipedia.org/wiki/List_of_space_groups
	std::istringstream iss(symbol);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, ' '))
    {
        if (token.compare("1")==0)
            continue;
    	tokens.push_back(token);
    }
    std::string reducedSymbol = boost::algorithm::join(tokens, " ");
    return reducedSymbol;
}

std::vector<std::string> SpaceGroupSymbols::getAllSymbols() const
{
	std::vector<std::string> symbols;
	symbols.reserve(_spaceGroupTables.size());
	for (const auto& value : _spaceGroupTables)
	{
		symbols.push_back(value.first);
	}
	return symbols;
}

std::string SpaceGroupSymbols::getFullSymbol(const std::string& symbol) const
{
   auto symbols = getAllSymbols();

   for(auto&& full_symbol: symbols)
       if (getReducedSymbol(symbol) == getReducedSymbol(full_symbol))
           return full_symbol;
}


} // Namespace Crystal
} // Namespace SX
