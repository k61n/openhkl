//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/crystal/TestSpaceGroup.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include "tables/crystal/SpaceGroup.h"
#include <map>
#include <string>

TEST_CASE("test/crystal/TestSpaceGroup.cpp", "")
{
    std::map<std::string, unsigned int> nElementsPerSpaceGroup = {
        {"P 1", 1},         {"P -1", 2},        {"P 2", 2},         {"P 21", 2},
        {"C 2", 4},         {"P m", 2},         {"P c", 2},         {"C m", 4},
        {"C c", 4},         {"P 2/m", 4},       {"P 21/m", 4},      {"C 2/m", 8},
        {"P 2/c", 4},       {"P 21/c", 4},      {"C 2/c", 8},       {"P 2 2 2", 4},
        {"P 2 2 21", 4},    {"P 21 21 2", 4},   {"P 21 21 21", 4},  {"C 2 2 21", 8},
        {"C 2 2 2", 8},     {"F 2 2 2", 16},    {"I 2 2 2", 8},     {"I 21 21 21", 8},
        {"P m m 2", 4},     {"P m c 21", 4},    {"P c c 2", 4},     {"P m a 2", 4},
        {"P c a 21", 4},    {"P n c 2", 4},     {"P m n 21", 4},    {"P b a 2", 4},
        {"P n a 21", 4},    {"P n n 2", 4},     {"C m m 2", 8},     {"C m c 21", 8},
        {"C c c 2", 8},     {"A m m 2", 8},     {"A b m 2", 8},     {"A m a 2", 8},
        {"A b a 2", 8},     {"F m m 2", 16},    {"F d d 2", 16},    {"I m m 2", 8},
        {"I b a 2", 8},     {"I m a 2", 8},     {"P m m m", 8},     {"P n n n", 8},
        {"P c c m", 8},     {"P b a n", 8},     {"P m m a", 8},     {"P n n a", 8},
        {"P m n a", 8},     {"P c c a", 8},     {"P b a m", 8},     {"P c c n", 8},
        {"P b c m", 8},     {"P n n m", 8},     {"P m m n", 8},     {"P b c n", 8},
        {"P b c a", 8},     {"P n m a", 8},     {"C m c m", 16},    {"C m c a", 16},
        {"C m m m", 16},    {"C c c m", 16},    {"C m m a", 16},    {"C c c a", 16},
        {"F m m m", 32},    {"F d d d", 32},    {"I m m m", 16},    {"I b a m", 16},
        {"I b c a", 16},    {"I m m a", 16},    {"P 4", 4},         {"P 41", 4},
        {"P 42", 4},        {"P 43", 4},        {"I 4", 8},         {"I 41", 8},
        {"P -4", 4},        {"I -4", 8},        {"P 4/m", 8},       {"P 42/m", 8},
        {"P 4/n", 8},       {"P 42/n", 8},      {"I 4/m", 16},      {"I 41/a", 16},
        {"P 4 2 2", 8},     {"P 4 21 2", 8},    {"P 41 2 2", 8},    {"P 41 21 2", 8},
        {"P 42 2 2", 8},    {"P 42 21 2", 8},   {"P 43 2 2", 8},    {"P 43 21 2", 8},
        {"I 4 2 2", 16},    {"I 41 2 2", 16},   {"P 4 m m", 8},     {"P 4 b m", 8},
        {"P 42 c m", 8},    {"P 42 n m", 8},    {"P 4 c c", 8},     {"P 4 n c", 8},
        {"P 42 m c", 8},    {"P 42 b c", 8},    {"I 4 m m", 16},    {"I 4 c m", 16},
        {"I 41 m d", 16},   {"I 41 c d", 16},   {"P -4 2 m", 8},    {"P -4 2 c", 8},
        {"P -4 21 m", 8},   {"P -4 21 c", 8},   {"P -4 m 2", 8},    {"P -4 c 2", 8},
        {"P -4 b 2", 8},    {"P -4 n 2", 8},    {"I -4 m 2", 16},   {"I -4 c 2", 16},
        {"I -4 2 m", 16},   {"I -4 2 d", 16},   {"P 4/m m m", 16},  {"P 4/m c c", 16},
        {"P 4/n b m", 16},  {"P 4/n n c", 16},  {"P 4/m b m", 16},  {"P 4/m n c", 16},
        {"P 4/n m m", 16},  {"P 4/n c c", 16},  {"P 42/m m c", 16}, {"P 42/m c m", 16},
        {"P 42/n b c", 16}, {"P 42/n n m", 16}, {"P 42/m b c", 16}, {"P 42/m n m", 16},
        {"P 42/n m c", 16}, {"P 42/n c m", 16}, {"I 4/m m m", 32},  {"I 4/m c m", 32},
        {"I 41/a m d", 32}, {"I 41/a c d", 32}, {"P 3", 3},         {"P 31", 3},
        {"P 32", 3},        {"R 3", 9},         {"P -3", 6},        {"R -3", 18},
        {"P 3 1 2", 6},     {"P 3 2 1", 6},     {"P 31 1 2", 6},    {"P 31 2 1", 6},
        {"P 32 1 2", 6},    {"P 32 2 1", 6},    {"R 3 2", 18},      {"P 3 m 1", 6},
        {"P 3 1 m", 6},     {"P 3 c 1", 6},     {"P 3 1 c", 6},     {"R 3 m", 18},
        {"R 3 c", 18},      {"P -3 1 m", 12},   {"P -3 1 c", 12},   {"P -3 m 1", 12},
        {"P -3 c 1", 12},   {"R -3 m", 36},     {"R -3 c", 36},     {"P 6", 6},
        {"P 61", 6},        {"P 65", 6},        {"P 62", 6},        {"P 64", 6},
        {"P 63", 6},        {"P -6", 6},        {"P 6/m", 12},      {"P 63/m", 12},
        {"P 6 2 2", 12},    {"P 61 2 2", 12},   {"P 65 2 2", 12},   {"P 62 2 2", 12},
        {"P 64 2 2", 12},   {"P 63 2 2", 12},   {"P 6 m m", 12},    {"P 6 c c", 12},
        {"P 63 c m", 12},   {"P 63 m c", 12},   {"P -6 m 2", 12},   {"P -6 c 2", 12},
        {"P -6 2 m", 12},   {"P -6 2 c", 12},   {"P 6/m m m", 24},  {"P 6/m c c", 24},
        {"P 63/m c m", 24}, {"P 63/m m c", 24}, {"P 2 3", 12},      {"F 2 3", 48},
        {"I 2 3", 24},      {"P 21 3", 12},     {"I 21 3", 24},     {"P m -3", 24},
        {"P n -3", 24},     {"F m -3", 96},     {"F d -3", 96},     {"I m -3", 48},
        {"P a -3", 24},     {"I a -3", 48},     {"P 4 3 2", 24},    {"P 42 3 2", 24},
        {"F 4 3 2", 96},    {"F 41 3 2", 96},   {"I 4 3 2", 48},    {"P 43 3 2", 24},
        {"P 41 3 2", 24},   {"I 41 3 2", 48},   {"P -4 3 m", 24},   {"F -4 3 m", 96},
        {"I -4 3 m", 48},   {"P -4 3 n", 24},   {"F -4 3 c", 96},   {"I -4 3 d", 48},
        {"P m -3 m", 48},   {"P n -3 n", 48},   {"P m -3 n", 48},   {"P n -3 m", 48},
        {"F m -3 m", 192},  {"F m -3 c", 192},  {"F d -3 m", 192},  {"F d -3 c", 192},
        {"I m -3 m", 96},   {"I a -3 d", 96},
    };

    CHECK(ohkl::SpaceGroup::symbols().size() == static_cast<size_t>(230));

    ohkl::SpaceGroup sg_pnma("P n m a");
    // Check that string generator strings are OK s
    CHECK(sg_pnma.generators().compare("-x+1/2,-y,z+1/2; -x,y+1/2,-z; -x,-y,-z") == 0);

    for (const auto& p : nElementsPerSpaceGroup) {
        ohkl::SpaceGroup sg(p.first);
        CHECK(sg.groupElements().size() == static_cast<size_t>(p.second));
    }

    // Test extinction conditions
    ohkl::SpaceGroup sg("F d -3 m");
    CHECK(sg.isExtinct({0, 0, 1}));
    CHECK(sg.isExtinct({0, 0, 2}));
    CHECK(sg.isExtinct({0, 0, 3}));
    CHECK(!sg.isExtinct({0, 0, 4}));
    CHECK(sg.isExtinct({1, 1, 2}));
    CHECK(!sg.isExtinct({1, 1, 3}));
    CHECK(sg.isExtinct({0, 2, 4}));
    CHECK(!sg.isExtinct({0, 2, 6}));

    // Test extinction conditions
    ohkl::SpaceGroup sg2("I 41/a");
    CHECK(!sg2.isExtinct({1, 2, 1}));
    CHECK(sg2.isExtinct({1, 2, 2}));
    CHECK(sg2.isExtinct({1, 2, 0}));
    CHECK(!sg2.isExtinct({2, 2, 0}));
    CHECK(sg2.isExtinct({0, 1, 2}));
    CHECK(!sg2.isExtinct({0, 2, 2}));
    CHECK(!sg2.isExtinct({1, 1, 2}));
    CHECK(sg2.isExtinct({1, 1, 3}));
    CHECK(sg2.isExtinct({0, 0, 2}));
    CHECK(sg2.isExtinct({1, -1, 0}));
    CHECK(!sg2.isExtinct({2, -2, 0}));

    ohkl::SpaceGroup sg3("I m -3 m");
    CHECK(sg3.bravaisType() == 'c');

    ohkl::SpaceGroup sg4("P n m a");
    CHECK(sg4.bravaisType() == 'o');

    ohkl::SpaceGroup sg5("F d -3 m");
    CHECK(sg5.bravaisType() == 'c');

    ohkl::SpaceGroup sg6("C 2/m");
    CHECK(sg6.bravaisType() == 'm');

    ohkl::SpaceGroup sg7("P 21/c");
    CHECK(sg7.bravaisType() == 'm');

    ohkl::SpaceGroup sg8("P 4 m m");
    CHECK(sg8.bravaisType() == 't');

    ohkl::SpaceGroup sg9("P 4/m m m");
    CHECK(sg9.bravaisType() == 't');

    ohkl::SpaceGroup sg10("P 63/m m c");
    CHECK(sg10.bravaisType() == 'h');

    ohkl::SpaceGroup sg11("P 3 2 1");
    CHECK(sg11.bravaisType() == 'h');

    ohkl::SpaceGroup sg12("P -1");
    CHECK(sg12.bravaisType() == 'a');

    ohkl::SpaceGroup sg13("R -3 c");
    CHECK(sg13.bravaisType() == 'h');

    // Test extinction conditions
    ohkl::SpaceGroup sg14("P 21 21 21");
    CHECK(sg14.isExtinct({3, 0, 0}));
    CHECK(sg14.isExtinct({0, 3, 0}));
    CHECK(sg14.isExtinct({0, 0, 3}));
    CHECK(!sg14.isExtinct({2, 0, 0}));
    CHECK(!sg14.isExtinct({0, 2, 0}));
    CHECK(!sg14.isExtinct({0, 0, 2}));

    // Test extinction conditions
    ohkl::SpaceGroup sg15("P 2 2 21");
    CHECK(!sg15.isExtinct({2, 0, 0}));
    CHECK(!sg15.isExtinct({0, 2, 0}));
    CHECK(!sg15.isExtinct({0, 0, 2}));
    CHECK(!sg15.isExtinct({3, 0, 0}));
    CHECK(!sg15.isExtinct({0, 3, 0}));
    CHECK(sg15.isExtinct({0, 0, 3}));
}
