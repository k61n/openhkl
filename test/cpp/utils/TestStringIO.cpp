//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/utils/TestStringIO.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <string>
#include <vector>

#include "base/utils/StringIO.h"

TEST_CASE("test/utils/TestStringIO.cpp", "")
{
    std::string str_to_trim("   blabla    ");

    CHECK(str_to_trim != "blabla");
    CHECK(ohkl::trim(str_to_trim) == "blabla");
    CHECK(str_to_trim != "blabla");

    std::string str_to_trim2("hello");
    CHECK(ohkl::trim(str_to_trim2) == "hello");

    std::string str_with_spaces("   bla      bla    ");
    CHECK(ohkl::clear_spaces(str_with_spaces) == "blabla");

    std::string str_to_compress("   bla      bla    ");
    CHECK(ohkl::compress(str_to_compress) == "bla bla");

    std::string str_to_compress2("nothing-to-compress");
    CHECK(ohkl::compress(str_to_compress2) == "nothing-to-compress");

    std::vector<std::string> to_be_joined({"toto", "titi", "tata"});
    CHECK(ohkl::join(to_be_joined, ",") == "toto,titi,tata");

    std::vector<std::string> to_be_joined2({"good morning", "everybody", "!"});
    CHECK(ohkl::join(to_be_joined2, " ") == "good morning everybody !");

    CHECK(
        ohkl::split("hello how are you ?", " ")
        != std::vector<std::string>({"hello", "how", "are", "you"}));
    CHECK(
        ohkl::split("hello how are you ?", " ")
        == std::vector<std::string>({"hello", "how", "are", "you", "?"}));
    CHECK(
        ohkl::split("hello ; how    ; are; you ;?", ";")
        == std::vector<std::string>({"hello", "how", "are", "you", "?"}));
    CHECK(
        ohkl::split("hello ; how    ; are; you ;?", " ")
        != std::vector<std::string>({"hello", "how", "are", "you", "?"}));

    const std::string str_to_lower {"TheCAPITALLetter 'd' is D "};
    CHECK(ohkl::lowerCase(str_to_lower) == "thecapitalletter 'd' is d ");

}
