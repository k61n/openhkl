//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      tables/crystal/SpaceGroup.cpp
//! @brief     Implements class SpaceGroup
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "tables/crystal/SpaceGroup.h"

#include "base/utils/StringIO.h"
#include "tables/crystal/UnitCell.h"

#include <algorithm>
#include <sstream>

namespace {
const std::vector<std::pair<std::string, std::string>> symmetry_table = {
    {"P 1", " x,y,z"},
    {"P -1", " -x,-y,-z"},
    {"P 2", " -x,y,-z"},
    {"P 21", " -x,y+1/2,-z"},
    {"C 2", " x+1/2,y+1/2,z; -x,y,-z"},
    {"P m", " x,-y,z"},
    {"P c", " x,-y,z+1/2"},
    {"C m", " x+1/2,y+1/2,z; x,-y,z"},
    {"C c", " x+1/2,y+1/2,z; x,-y,z+1/2"},
    {"P 2/m", " -x,y,-z; -x,-y,-z"},
    {"P 21/m", " -x,y+1/2,-z; -x,-y,-z"},
    {"C 2/m", " x+1/2,y+1/2,z; -x,y,-z; -x,-y,-z"},
    {"P 2/c", " -x,y,-z+1/2; -x,-y,-z"},
    {"P 21/c", " -x,y+1/2,-z+1/2; -x,-y,-z"},
    {"C 2/c", " x+1/2,y+1/2,z; -x,y,-z+1/2; -x,-y,-z"},
    {"P 2 2 2", " -x,-y,z; -x,y,-z"},
    {"P 2 2 21", " -x,-y,z+1/2; -x,y,-z+1/2"},
    {"P 21 21 2", " -x,-y,z; -x+1/2,y+1/2,-z"},
    {"P 21 21 21", " -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2"},
    {"C 2 2 21", " x+1/2,y+1/2,z; -x,-y,z+1/2; -x,y,-z+1/2"},
    {"C 2 2 2", " x+1/2,y+1/2,z; -x,-y,z; -x,y,-z"},
    {"F 2 2 2", " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z"},
    {"I 2 2 2", " x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z"},
    {"I 21 21 21", " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2"},
    {"P m m 2", " -x,-y,z; x,-y,z"},
    {"P m c 21", " -x,-y,z+1/2; x,-y,z+1/2"},
    {"P c c 2", " -x,-y,z; x,-y,z+1/2"},
    {"P m a 2", " -x,-y,z; x+1/2,-y,z"},
    {"P c a 21", " -x,-y,z+1/2; x+1/2,-y,z"},
    {"P n c 2", " -x,-y,z; x,-y+1/2,z+1/2"},
    {"P m n 21", " -x+1/2,-y,z+1/2; x+1/2,-y,z+1/2"},
    {"P b a 2", " -x,-y,z; x+1/2,-y+1/2,z"},
    {"P n a 21", " -x,-y,z+1/2; x+1/2,-y+1/2,z"},
    {"P n n 2", " -x,-y,z; x+1/2,-y+1/2,z+1/2"},
    {"C m m 2", " x+1/2,y+1/2,z; -x,-y,z; x,-y,z"},
    {"C m c 21", " x+1/2,y+1/2,z; -x,-y,z+1/2; x,-y,z+1/2"},
    {"C c c 2", " x+1/2,y+1/2,z; -x,-y,z; x,-y,z+1/2"},
    {"A m m 2", " x,y+1/2,z+1/2; -x,-y,z; x,-y,z"},
    {"A b m 2", " x,y+1/2,z+1/2; -x,-y,z; x,-y+1/2,z"},
    {"A m a 2", " x,y+1/2,z+1/2; -x,-y,z; x+1/2,-y,z"},
    {"A b a 2", " x,y+1/2,z+1/2; -x,-y,z; x+1/2,-y+1/2,z"},
    {"F m m 2", " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; x,-y,z"},
    {"F d d 2", " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; x+1/4,-y+1/4,z+1/4"},
    {"I m m 2", " x+1/2,y+1/2,z+1/2; -x,-y,z; x,-y,z"},
    {"I b a 2", " x+1/2,y+1/2,z+1/2; -x,-y,z; x+1/2,-y+1/2,z"},
    {"I m a 2", " x+1/2,y+1/2,z+1/2; -x,-y,z; x+1/2,-y,z"},
    {"P m m m", " -x,-y,z; -x,y,-z; -x,-y,-z"},
    {"P n n n", " -x+1/2,-y+1/2,z; -x+1/2,y,-z+1/2; -x,-y,-z"},
    {"P c c m", " -x,-y,z; -x,y,-z+1/2; -x,-y,-z"},
    {"P b a n", " -x+1/2,-y+1/2,z; -x+1/2,y,-z; -x,-y,-z"},
    {"P m m a", " -x+1/2,-y,z; -x,y,-z; -x,-y,-z"},
    {"P n n a", " -x+1/2,-y,z; -x+1/2,y+1/2,-z+1/2; -x,-y,-z"},
    {"P m n a", " -x+1/2,-y,z+1/2; -x+1/2,y,-z+1/2; -x,-y,-z"},
    {"P c c a", " -x+1/2,-y,z; -x,y,-z+1/2; -x,-y,-z"},
    {"P b a m", " -x,-y,z; -x+1/2,y+1/2,-z; -x,-y,-z"},
    {"P c c n", " -x+1/2,-y+1/2,z; -x,y+1/2,-z+1/2; -x,-y,-z"},
    {"P b c m", " -x,-y,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
    {"P n n m", " -x,-y,z; -x+1/2,y+1/2,-z+1/2; -x,-y,-z"},
    {"P m m n", " -x+1/2,-y+1/2,z; -x,y+1/2,-z; -x,-y,-z"},
    {"P b c n", " -x+1/2,-y+1/2,z+1/2; -x,y,-z+1/2; -x,-y,-z"},
    {"P b c a", " -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
    {"P n m a", " -x+1/2,-y,z+1/2; -x,y+1/2,-z; -x,-y,-z"},
    {"C m c m", " x+1/2,y+1/2,z; -x,-y,z+1/2; -x,y,-z+1/2; -x,-y,-z"},
    {"C m c a", " x+1/2,y+1/2,z; -x,-y+1/2,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
    {"C m m m", " x+1/2,y+1/2,z; -x,-y,z; -x,y,-z; -x,-y,-z"},
    {"C c c m", " x+1/2,y+1/2,z; -x,-y,z; -x,y,-z+1/2; -x,-y,-z"},
    {"C m m a", " x+1/2,y+1/2,z; -x,-y+1/2,z; -x,y+1/2,-z; -x,-y,-z"},
    {"C c c a", " x+1/2,y+1/2,z; -x+1/2,-y,z; -x,y,-z+1/2; -x,-y,-z"},
    {"F m m m", " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; -x,-y,-z"},
    {"F d d d",
     " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x+3/4,-y+3/4,z; "
     "-x+3/4,y,-z+3/4; -x,-y,-z"},
    {"I m m m", " x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; -x,-y,-z"},
    {"I b a m", " x+1/2,y+1/2,z+1/2; -x,-y,z; -x+1/2,y+1/2,-z; -x,-y,-z"},
    {"I b c a", " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
    {"I m m a", " x+1/2,y+1/2,z+1/2; -x,-y+1/2,z; -x,y+1/2,-z; -x,-y,-z"},
    {"P 4", " -x,-y,z; -y,x,z"},
    {"P 41", " -x,-y,z+1/2; -y,x,z+1/4"},
    {"P 42", " -x,-y,z; -y,x,z+1/2"},
    {"P 43", " -x,-y,z+1/2; -y,x,z+3/4"},
    {"I 4", " x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z"},
    {"I 41", " x+1/2,y+1/2,z+1/2; -x+1/2,-y+1/2,z+1/2; -y,x+1/2,z+1/4"},
    {"P -4", " -x,-y,z; y,-x,-z"},
    {"I -4", " x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z"},
    {"P 4/m", " -x,-y,z; -y,x,z; -x,-y,-z"},
    {"P 42/m", " -x,-y,z; -y,x,z+1/2; -x,-y,-z"},
    {"P 4/n", " -x+1/2,-y+1/2,z; -y+1/2,x,z; -x,-y,-z"},
    {"P 42/n", " -x+1/2,-y+1/2,z; -y,x+1/2,z+1/2; -x,-y,-z"},
    {"I 4/m", " x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; -x,-y,-z"},
    {"I 41/a", " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -y+3/4,x+1/4,z+1/4; -x,-y,-z"},
    {"P 4 2 2", " -x,-y,z; -y,x,z; -x,y,-z"},
    {"P 4 21 2", " -x,-y,z; -y+1/2,x+1/2,z; -x+1/2,y+1/2,-z"},
    {"P 41 2 2", " -x,-y,z+1/2; -y,x,z+1/4; -x,y,-z"},
    {"P 41 21 2", " -x,-y,z+1/2; -y+1/2,x+1/2,z+1/4; -x+1/2,y+1/2,-z+1/4"},
    {"P 42 2 2", " -x,-y,z; -y,x,z+1/2; -x,y,-z"},
    {"P 42 21 2", " -x,-y,z; -y+1/2,x+1/2,z+1/2; -x+1/2,y+1/2,-z+1/2"},
    {"P 43 2 2", " -x,-y,z+1/2; -y,x,z+3/4; -x,y,-z"},
    {"P 43 21 2", " -x,-y,z+1/2; -y+1/2,x+1/2,z+3/4; -x+1/2,y+1/2,-z+3/4"},
    {"I 4 2 2", " x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; -x,y,-z"},
    {"I 41 2 2",
     " x+1/2,y+1/2,z+1/2; -x+1/2,-y+1/2,z+1/2; -y,x+1/2,z+1/4; "
     "-x+1/2,y,-z+3/4"},
    {"P 4 m m", " -x,-y,z; -y,x,z; x,-y,z"},
    {"P 4 b m", " -x,-y,z; -y,x,z; x+1/2,-y+1/2,z"},
    {"P 42 c m", " -x,-y,z; -y,x,z+1/2; x,-y,z+1/2"},
    {"P 42 n m", " -x,-y,z; -y+1/2,x+1/2,z+1/2; x+1/2,-y+1/2,z+1/2"},
    {"P 4 c c", " -x,-y,z; -y,x,z; x,-y,z+1/2"},
    {"P 4 n c", " -x,-y,z; -y,x,z; x+1/2,-y+1/2,z+1/2"},
    {"P 42 m c", " -x,-y,z; -y,x,z+1/2; x,-y,z"},
    {"P 42 b c", " -x,-y,z; -y,x,z+1/2; x+1/2,-y+1/2,z"},
    {"I 4 m m", " x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; x,-y,z"},
    {"I 4 c m", " x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; x,-y,z+1/2"},
    {"I 41 m d", " x+1/2,y+1/2,z+1/2; -x+1/2,-y+1/2,z+1/2; -y,x+1/2,z+1/4; x,-y,z"},
    {"I 41 c d", " x+1/2,y+1/2,z+1/2; -x+1/2,-y+1/2,z+1/2; -y,x+1/2,z+1/4; x,-y,z+1/2"},
    {"P -4 2 m", " -x,-y,z; y,-x,-z; -x,y,-z"},
    {"P -4 2 c", " -x,-y,z; y,-x,-z; -x,y,-z+1/2"},
    {"P -4 21 m", " -x,-y,z; y,-x,-z; -x+1/2,y+1/2,-z"},
    {"P -4 21 c", " -x,-y,z; y,-x,-z; -x+1/2,y+1/2,-z+1/2"},
    {"P -4 m 2", " -x,-y,z; y,-x,-z; x,-y,z"},
    {"P -4 c 2", " -x,-y,z; y,-x,-z; x,-y,z+1/2"},
    {"P -4 b 2", " -x,-y,z; y,-x,-z; x+1/2,-y+1/2,z"},
    {"P -4 n 2", " -x,-y,z; y,-x,-z; x+1/2,-y+1/2,z+1/2"},
    {"I -4 m 2", " x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z; x,-y,z"},
    {"I -4 c 2", " x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z; x,-y,z+1/2"},
    {"I -4 2 m", " x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z; -x,y,-z"},
    {"I -4 2 d", " x+1/2,y+1/2,z+1/2; -x,-y,z; y,-x,-z; -x+1/2,y,-z+3/4"},
    {"P 4/m m m", " -x,-y,z; -y,x,z; -x,y,-z; -x,-y,-z"},
    {"P 4/m c c", " -x,-y,z; -y,x,z; -x,y,-z+1/2; -x,-y,-z"},
    {"P 4/n b m", " -x+1/2,-y+1/2,z; -y+1/2,x,z; -x+1/2,y,-z; -x,-y,-z"},
    {"P 4/n n c", " -x+1/2,-y+1/2,z; -y+1/2,x,z; -x+1/2,y,-z+1/2; -x,-y,-z"},
    {"P 4/m b m", " -x,-y,z; -y,x,z; -x+1/2,y+1/2,-z; -x,-y,-z"},
    {"P 4/m n c", " -x,-y,z; -y,x,z; -x+1/2,y+1/2,-z+1/2; -x,-y,-z"},
    {"P 4/n m m", " -x+1/2,-y+1/2,z; -y+1/2,x,z; -x,y+1/2,-z; -x,-y,-z"},
    {"P 4/n c c", " -x+1/2,-y+1/2,z; -y+1/2,x,z; -x,y+1/2,-z+1/2; -x,-y,-z"},
    {"P 42/m m c", " -x,-y,z; -y,x,z+1/2; -x,y,-z; -x,-y,-z"},
    {"P 42/m c m", " -x,-y,z; -y,x,z+1/2; -x,y,-z+1/2; -x,-y,-z"},
    {"P 42/n b c", " -x+1/2,-y+1/2,z; -y+1/2,x,z+1/2; -x+1/2,y,-z; -x,-y,-z"},
    {"P 42/n n m", " -x+1/2,-y+1/2,z; -y+1/2,x,z+1/2; -x+1/2,y,-z+1/2; -x,-y,-z"},
    {"P 42/m b c", " -x,-y,z; -y,x,z+1/2; -x+1/2,y+1/2,-z; -x,-y,-z"},
    {"P 42/m n m", " -x,-y,z; -y+1/2,x+1/2,z+1/2; -x+1/2,y+1/2,-z+1/2; -x,-y,-z"},
    {"P 42/n m c", " -x+1/2,-y+1/2,z; -y+1/2,x,z+1/2; -x,y+1/2,-z; -x,-y,-z"},
    {"P 42/n c m", " -x+1/2,-y+1/2,z; -y+1/2,x,z+1/2; -x,y+1/2,-z+1/2; -x,-y,-z"},
    {"I 4/m m m", " x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; -x,y,-z; -x,-y,-z"},
    {"I 4/m c m", " x+1/2,y+1/2,z+1/2; -x,-y,z; -y,x,z; -x,y,-z+1/2; -x,-y,-z"},
    {"I 41/a m d",
     " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -y+1/4,x+3/4,z+1/4; "
     "-x+1/2,y,-z+1/2; -x,-y,-z"},
    {"I 41/a c d",
     " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -y+1/4,x+3/4,z+1/4; "
     "-x+1/2,y,-z; -x,-y,-z"},
    {"P 3", " -y,x-y,z"},
    {"P 31", " -y,x-y,z+1/3"},
    {"P 32", " -y,x-y,z+2/3"},
    {"R 3", " x+1/3,y+2/3,z+2/3; -y,x-y,z"},
    {"P -3", " -y,x-y,z; -x,-y,-z"},
    {"R -3", " x+1/3,y+2/3,z+2/3; -y,x-y,z; -x,-y,-z"},
    {"P 3 1 2", " -y,x-y,z; -y,-x,-z"},
    {"P 3 2 1", " -y,x-y,z; y,x,-z"},
    {"P 31 1 2", " -y,x-y,z+1/3; -y,-x,-z+2/3"},
    {"P 31 2 1", " -y,x-y,z+1/3; y,x,-z"},
    {"P 32 1 2", " -y,x-y,z+2/3; -y,-x,-z+1/3"},
    {"P 32 2 1", " -y,x-y,z+2/3; y,x,-z"},
    {"R 3 2", " x+1/3,y+2/3,z+2/3; -y,x-y,z; y,x,-z"},
    {"P 3 m 1", " -y,x-y,z; -y,-x,z"},
    {"P 3 1 m", " -y,x-y,z; y,x,z"},
    {"P 3 c 1", " -y,x-y,z; -y,-x,z+1/2"},
    {"P 3 1 c", " -y,x-y,z; y,x,z+1/2"},
    {"R 3 m", " x+1/3,y+2/3,z+2/3; -y,x-y,z; -y,-x,z"},
    {"R 3 c", " x+1/3,y+2/3,z+2/3; -y,x-y,z; -y,-x,z+1/2"},
    {"P -3 1 m", " -y,x-y,z; -y,-x,-z; -x,-y,-z"},
    {"P -3 1 c", " -y,x-y,z; -y,-x,-z+1/2; -x,-y,-z"},
    {"P -3 m 1", " -y,x-y,z; y,x,-z; -x,-y,-z"},
    {"P -3 c 1", " -y,x-y,z; y,x,-z+1/2; -x,-y,-z"},
    {"R -3 m", " x+1/3,y+2/3,z+2/3; -y,x-y,z; y,x,-z; -x,-y,-z"},
    {"R -3 c", " x+1/3,y+2/3,z+2/3; -y,x-y,z; y,x,-z+1/2; -x,-y,-z"},
    {"P 6", " -y,x-y,z; -x,-y,z"},
    {"P 61", " -y,x-y,z+1/3; -x,-y,z+1/2"},
    {"P 65", " -y,x-y,z+2/3; -x,-y,z+1/2"},
    {"P 62", " -y,x-y,z+2/3; -x,-y,z"},
    {"P 64", " -y,x-y,z+1/3; -x,-y,z"},
    {"P 63", " -y,x-y,z; -x,-y,z+1/2"},
    {"P -6", " -y,x-y,z; x,y,-z"},
    {"P 6/m", " -y,x-y,z; -x,-y,z; -x,-y,-z"},
    {"P 63/m", " -y,x-y,z; -x,-y,z+1/2; -x,-y,-z"},
    {"P 6 2 2", " -y,x-y,z; -x,-y,z; y,x,-z"},
    {"P 61 2 2", " -y,x-y,z+1/3; -x,-y,z+1/2; y,x,-z+1/3"},
    {"P 65 2 2", " -y,x-y,z+2/3; -x,-y,z+1/2; y,x,-z+2/3"},
    {"P 62 2 2", " -y,x-y,z+2/3; -x,-y,z; y,x,-z+2/3"},
    {"P 64 2 2", " -y,x-y,z+1/3; -x,-y,z; y,x,-z+1/3"},
    {"P 63 2 2", " -y,x-y,z; -x,-y,z+1/2; y,x,-z"},
    {"P 6 m m", " -y,x-y,z; -x,-y,z; -y,-x,z"},
    {"P 6 c c", " -y,x-y,z; -x,-y,z; -y,-x,z+1/2"},
    {"P 63 c m", " -y,x-y,z; -x,-y,z+1/2; -y,-x,z+1/2"},
    {"P 63 m c", " -y,x-y,z; -x,-y,z+1/2; -y,-x,z"},
    {"P -6 m 2", " -y,x-y,z; x,y,-z; -y,-x,z"},
    {"P -6 c 2", " -y,x-y,z; x,y,-z+1/2; -y,-x,z+1/2"},
    {"P -6 2 m", " -y,x-y,z; x,y,-z; y,x,-z"},
    {"P -6 2 c", " -y,x-y,z; x,y,-z+1/2; y,x,-z"},
    {"P 6/m m m", " -y,x-y,z; -x,-y,z; y,x,-z; -x,-y,-z"},
    {"P 6/m c c", " -y,x-y,z; -x,-y,z; y,x,-z+1/2; -x,-y,-z"},
    {"P 63/m c m", " -y,x-y,z; -x,-y,z+1/2; y,x,-z+1/2; -x,-y,-z"},
    {"P 63/m m c", " -y,x-y,z; -x,-y,z+1/2; y,x,-z; -x,-y,-z"},
    {"P 2 3", " -x,-y,z; -x,y,-z; z,x,y"},
    {"F 2 3", " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y"},
    {"I 2 3", " x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y"},
    {"P 21 3", " -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y"},
    {"I 21 3", " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y"},
    {"P m -3", " -x,-y,z; -x,y,-z; z,x,y; -x,-y,-z"},
    {"P n -3", " -x+1/2,-y+1/2,z; -x+1/2,y,-z+1/2; z,x,y; -x,-y,-z"},
    {"F m -3", " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; -x,-y,-z"},
    {"F d -3",
     " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x+1/4,-y+1/4,z; "
     "-x+1/4,y,-z+1/4; z,x,y; -x,-y,-z"},
    {"I m -3", " x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y; -x,-y,-z"},
    {"P a -3", " -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; -x,-y,-z"},
    {"I a -3", " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; -x,-y,-z"},
    {"P 4 3 2", " -x,-y,z; -x,y,-z; z,x,y; y,x,-z"},
    {"P 42 3 2", " -x,-y,z; -x,y,-z; z,x,y; y+1/2,x+1/2,-z+1/2"},
    {"F 4 3 2", " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,-z"},
    {"F 41 3 2",
     " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y+1/2,z+1/2; "
     "-x+1/2,y+1/2,-z; z,x,y; y+3/4,x+1/4,-z+3/4"},
    {"I 4 3 2", " x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,-z"},
    {"P 43 3 2", " -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; y+1/4,x+3/4,-z+3/4"},
    {"P 41 3 2", " -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; y+3/4,x+1/4,-z+1/4"},
    {"I 41 3 2",
     " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; "
     "y+3/4,x+1/4,-z+1/4"},
    {"P -4 3 m", " -x,-y,z; -x,y,-z; z,x,y; y,x,z"},
    {"F -4 3 m", " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,z"},
    {"I -4 3 m", " x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,z"},
    {"P -4 3 n", " -x,-y,z; -x,y,-z; z,x,y; y+1/2,x+1/2,z+1/2"},
    {"F -4 3 c",
     " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; "
     "y+1/2,x+1/2,z+1/2"},
    {"I -4 3 d",
     " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; "
     "y+1/4,x+1/4,z+1/4"},
    {"P m -3 m", " -x,-y,z; -x,y,-z; z,x,y; y,x,-z; -x,-y,-z"},
    {"P n -3 n", " -x+1/2,-y+1/2,z; -x+1/2,y,-z+1/2; z,x,y; y,x,-z+1/2; -x,-y,-z"},
    {"P m -3 n", " -x,-y,z; -x,y,-z; z,x,y; y+1/2,x+1/2,-z+1/2; -x,-y,-z"},
    {"P n -3 m", " -x+1/2,-y+1/2,z; -x+1/2,y,-z+1/2; z,x,y; y+1/2,x+1/2,-z; -x,-y,-z"},
    {"F m -3 m",
     " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; "
     "y,x,-z; -x,-y,-z"},
    {"F m -3 c",
     " x+1/2,y+1/2,z; x+1/2,y,z+1/2; -x,-y,z; -x,y,-z; z,x,y; "
     "y+1/2,x+1/2,-z+1/2; -x,-y,-z"},
    {"F d -3 m",
     " x+1/2,y+1/2,z; "
     "x+1/2,y,z+1/2;-x+3/4,-y+1/4,z+1/2;-x+1/4,y+1/2,-z+3/"
     "4;z,x,y;y+3/4,x+1/4,-z+1/2;-x,-y,-z"},
    {"F d -3 c",
     " x+1/2,y+1/2,z; "
     "x+1/2,y,z+1/2;-x+1/4,-y+3/4,z+1/2;-x+3/4,y+1/2,-z+1/"
     "4;z,x,y;y+3/4,x+1/4,-z;-x,-y,-z"},
    {"I m -3 m", " x+1/2,y+1/2,z+1/2; -x,-y,z; -x,y,-z; z,x,y; y,x,-z; -x,-y,-z"},
    {"I a -3 d",
     " x+1/2,y+1/2,z+1/2; -x+1/2,-y,z+1/2; -x,y+1/2,-z+1/2; z,x,y; "
     "y+3/4,x+1/4,-z+1/4; -x,-y,-z"}};

void reduceSymbol(std::string& symbol)
{
    // Only in the following cases we keep the "1"s:
    if (symbol == "P 1"
      || symbol == "P 3 1 m"
      || symbol == "P 3 m 1"
      || symbol == "P -3 1 m"
      || symbol == "P -3 m 1"
      || symbol == "P 3 1 c"
      || symbol == "P 3 c 1"
      || symbol == "P -3 1 c"
      || symbol == "P -3 c 1"
      || symbol == "P 32 2 1"
      || symbol == "P 32 1 2"
      || symbol == "P 31 1 2"
      || symbol == "P 31 2 1"
      || symbol == "P 3 1 2"
      || symbol == "P 3 2 1")
        return;

    // Otherwise throw away every "1" to produce the short name for Bravais
    // see https://en.wikipedia.org/wiki/List_of_space_groups
    std::istringstream iss(symbol);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(iss, token, ' ')) {
        if (token.compare("1") == 0)
            continue;
        tokens.push_back(token);
    }

    symbol = nsx::join(tokens, " ");
}

} // namespace


namespace nsx {

std::vector<std::string> SpaceGroup::symbols()
{
    std::vector<std::string> symbols;
    symbols.reserve(symmetry_table.size());
    std::transform(
        symmetry_table.begin(), symmetry_table.end(), std::back_inserter(symbols),
        [](const std::pair<std::string, std::string>& s) -> std::string { return s.first; });
    return symbols;
}

SpaceGroup::SpaceGroup(std::string symbol)
{
    _symbol = compress(trim(symbol));
    reduceSymbol(_symbol);

    auto find_symbol = [this](const std::pair<std::string, std::string>& s) {
        return s.first.compare(this->_symbol) == 0;
    };
    auto it = std::find_if(symmetry_table.begin(), symmetry_table.end(), find_symbol);

    if (it == symmetry_table.end())
        throw std::runtime_error("Unknown space group: " + _symbol);

    _generators = trim(it->second);
    generateGroupElements();
}

char SpaceGroup::bravaisType() const
{
    std::vector<int> nrot(13, 0);
    int nPureTrans(0);
    const int isCentro = isCentrosymmetric() ? 2 : 1;

    for (const auto& g : _groupElements) {
        assert(g.getAxisOrder() + 6 >= 0);
        size_t idx = size_t(g.getAxisOrder() + 6);
        ++nrot[idx];
        if (g.isPureTranslation())
            nPureTrans++;
    }
    const int fact = (1 + nPureTrans) * isCentro;

    // Cubic
    if ((nrot[3] + nrot[9]) == 8 * fact)
        return 'c';
    // Hexagonal
    if ((nrot[0] + nrot[12]) == 2 * fact)
        return 'h';
    // Trigonal
    if ((nrot[3] + nrot[9]) == 2 * fact)
        return 'h';
    // Tetragonal
    if ((nrot[2] + nrot[10]) == 2 * fact)
        return 't';
    // Orthorhombic
    if ((nrot[4] + nrot[8]) == 3 * fact)
        return 'o';
    // Monoclinic
    if ((nrot[4] + nrot[8]) == fact)
        return 'm';
    // Triclinic, only remaining case
    return 'a';
}

double SpaceGroup::fractionExtinct(const MillerIndexList& hkls) const
{
    unsigned int extinct = 0;
    unsigned int total = hkls.size();

    for (const auto& hkl : hkls) {
        if (isExtinct(hkl)) {
            ++extinct;
        }
    }
    return static_cast<double>(extinct) / static_cast<double>(total);
}

std::string SpaceGroup::bravaisTypeSymbol() const
{
    std::string bravais;
    bravais += bravaisType();
    bravais += _symbol[0];

    return bravais;
}

int SpaceGroup::id() const
{
    auto find_symbol = [this](const std::pair<std::string, std::string>& s) {
        return s.first.compare(this->_symbol) == 0;
    };
    auto it = std::find_if(symmetry_table.begin(), symmetry_table.end(), find_symbol);
    return std::distance(symmetry_table.begin(), it);
}

bool SpaceGroup::isCentrosymmetric() const
{
    for (const auto& g : _groupElements) {
        if (g.getAxisOrder() == -1)
            return true;
    }
    return false;
}

const std::string& SpaceGroup::symbol() const
{
    return _symbol;
}

const std::string& SpaceGroup::generators() const
{
    return _generators;
}

const SymOpList& SpaceGroup::groupElements() const
{
    return _groupElements;
}

void SpaceGroup::generateGroupElements()
{
    _groupElements.clear();
    SymOpList generators;
    std::vector<std::string> gens = split(_generators, ";");
    generators.reserve(gens.size() + 1);
    generators.emplace_back(SymOp(affineTransformation::Identity()));

    for (const auto& g : gens) {
        auto&& gen = SymOp(g);
        generators.emplace_back(gen);
    }

    _groupElements.emplace_back(SymOp(affineTransformation::Identity()));
    unsigned int oldSize = 0;

    while (oldSize != _groupElements.size()) {
        oldSize = _groupElements.size();
        for (unsigned int i = 0; i < _groupElements.size(); ++i) {
            for (const auto& g : generators) {
                auto newElement = _groupElements[i] * g;
                auto it = std::find(_groupElements.begin(), _groupElements.end(), newElement);
                if (it == _groupElements.end())
                    _groupElements.push_back(newElement);
            }
        }
    }
}

bool SpaceGroup::isExtinct(const MillerIndex& hkl) const
{
    // todo(jonathan): improve this routine? need a journal reference
    // check that this produces results consistent with
    // http://www.ccp14.ac.uk/ccp/web-mirrors/powdcell/a_v/v_1/powder/details/extinct.htm

    Eigen::Vector3d hkld = hkl.rowVector().transpose().cast<double>();

    for (const auto& element : _groupElements) {
        if (element.hasTranslation()) {
            Eigen::Vector3d t = element.getTranslationPart();
            double scalar = t.dot(hkld);
            std::complex<double> prefactor =
                1.0 + std::exp(std::complex<double>(0, 2 * M_PI) * scalar);
            if (std::abs(prefactor) < 1e-3) {
                // if (std::abs(std::remainder(scalar,1.0))>1e-3)
                Eigen::Vector3d rhkl = element.getRotationPart() * hkld;
                if (std::abs(rhkl(0) - hkld(0)) < 1e-3 && std::abs(rhkl(1) - hkld(1)) < 1e-3
                    && std::abs(rhkl(2) - hkld(2)) < 1e-3) {
                    return true;
                }
            }
        }
    }
    return false;
}

void SpaceGroup::print(std::ostream& os) const
{
    os << "Symmetry elements of space group " << _symbol << std::endl;
    for (const auto& g : _groupElements)
        os << g << " ; ";
    os << std::endl;
}

std::ostream& operator<<(std::ostream& os, const SpaceGroup& sg)
{
    sg.print(os);
    return os;
}

bool SpaceGroup::isEquivalent(const MillerIndex& hkl1, const MillerIndex& hkl2, bool friedel) const
{
    const auto& elements = groupElements();
    const double eps = 1e-6;

    const Eigen::RowVector3d& hkl1d = hkl1.rowVector().cast<double>();
    const Eigen::RowVector3d& hkl2d = hkl2.rowVector().cast<double>();

    // note: since rotation preserves the norm, we can reject early:
    const double norm_1 = hkl1d.squaredNorm();
    const double norm_2 = hkl2d.squaredNorm();

    if (std::abs(norm_1 - norm_2) > eps)
        return false;

    for (const auto& element : elements) {
        // todo(jonathan): check that this edit is correct!
        const Eigen::Matrix3d rotation = element.getRotationPart().transpose();
        const Eigen::RowVector3d rotated = hkl1d * rotation;

        if (std::max((rotated - hkl1d).maxCoeff(), (hkl1d - rotated).maxCoeff()) < eps)
            return true;

        if (friedel && std::max((rotated + hkl1d).maxCoeff(), (-hkl1d - rotated).maxCoeff()) < eps)
            return true;
    }
    return false;
}

bool SpaceGroup::isFriedelEquivalent(const MillerIndex& hkl1, const MillerIndex& hkl2) const
{
    const auto& elements = _groupElements;
    Eigen::RowVector3d rotated;

    const Eigen::RowVector3d& hkl1d = hkl1.rowVector().cast<double>();
    const Eigen::RowVector3d& hkl2d = hkl2.rowVector().cast<double>();

    for (const auto& element : elements) {
        // todo(jonathan): check that this edit is correct!
        // rotated = element.getMatrix()*Eigen::Vector3d(h1,k1,l1);
        rotated = hkl1d * element.getRotationPart().transpose();

        if (std::abs(rotated[0] - hkl2d[0]) < 1e-6 && std::abs(rotated[1] - hkl2d[1]) < 1e-6
            && std::abs(rotated[2] - hkl2d[2]) < 1e-6)
            return true;
        // compare against Friedel reflection
        if (std::abs(rotated[0] + hkl2d[0]) < 1e-6 && std::abs(rotated[1] + hkl2d[1]) < 1e-6
            && std::abs(rotated[2] + hkl2d[2]) < 1e-6)
            return true;
    }
    return false;
}

std::string SpaceGroup::toString() const
{
    return _symbol;
}

} // namespace nsx
