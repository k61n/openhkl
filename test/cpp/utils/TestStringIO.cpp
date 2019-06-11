#include "test/cpp/catch.hpp"

#include <string>
#include <vector>

#include "base/utils/StringIO.h"

TEST_CASE("test/utils/TestStringIO.cpp", "")
{

    std::string str_to_trim("   blabla    ");

    CHECK(str_to_trim != "blabla");
    CHECK(nsx::trim(str_to_trim) == "blabla");
    CHECK(str_to_trim != "blabla");

    std::string str_to_trim2("hello");
    CHECK(nsx::trim(str_to_trim2) == "hello");

    std::string str_with_spaces("   bla      bla    ");
    CHECK(nsx::clear_spaces(str_with_spaces) == "blabla");

    std::string str_to_compress("   bla      bla    ");
    CHECK(nsx::compress(str_to_compress) == "bla bla");

    std::string str_to_compress2("nothing-to-compress");
    CHECK(nsx::compress(str_to_compress2) == "nothing-to-compress");

    std::vector<std::string> to_be_joined({"toto", "titi", "tata"});
    CHECK(nsx::join(to_be_joined, ",") == "toto,titi,tata");

    std::vector<std::string> to_be_joined2({"good morning", "everybody", "!"});
    CHECK(nsx::join(to_be_joined2, " ") == "good morning everybody !");

    CHECK(
        nsx::split("hello how are you ?", " ")
        != std::vector<std::string>({"hello", "how", "are", "you"}));
    CHECK(
        nsx::split("hello how are you ?", " ")
        == std::vector<std::string>({"hello", "how", "are", "you", "?"}));
    CHECK(
        nsx::split("hello ; how    ; are; you ;?", ";")
        == std::vector<std::string>({"hello", "how", "are", "you", "?"}));
    CHECK(
        nsx::split("hello ; how    ; are; you ;?", " ")
        != std::vector<std::string>({"hello", "how", "are", "you", "?"}));
}
