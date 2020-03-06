//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/utils/TestVariant.cpp
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

#include "base/utils/Variant.h"

TEST_CASE("test/utils/TestVariant.cpp", "")
{
    // Default constructor
    nsx::Variant<int, double, std::string> v1;
    CHECK(v1.as<int>() == 0);

    // Constructor from values

    nsx::Variant<int, double, std::string> v2(20.0);
    CHECK(v2.as<double>() == Approx(20.0).epsilon(1.0e-12));

    nsx::Variant<int, double, std::string> v3(std::string("hello"));
    CHECK(v3.as<std::string>().compare("hello") == 0);

    const int ci(50);
    nsx::Variant<int, double, std::string> v4(ci);
    CHECK(v4.as<int>() == 50);

    double d(-25.0);
    nsx::Variant<int, double, std::string> v5(d);
    CHECK(v5.as<double>() == Approx(-25.0).epsilon(1.0e-12));

    // Move/Copy constructors from another Variant

    nsx::Variant<int, double, std::string> v6(v1);
    CHECK(v6.as<int>() == 0);

    nsx::Variant<int, double, std::string> v7(nsx::Variant<int, double, std::string>(40));
    CHECK(v7.as<int>() == 40);

    const nsx::Variant<int, double, std::string> cv(17.0);
    nsx::Variant<int, double, std::string> v8(cv);
    CHECK(v8.as<double>() == Approx(17.0).epsilon(1.0e-12));

    nsx::Variant<int, double, std::string> v9(nsx::Variant<int, double, std::string>(19));
    CHECK(v9.as<int>() == 19);

    // Move/Copy constructors from a subset Variant

    nsx::Variant<int, double> sv(19.0);
    nsx::Variant<int, double, std::string> v10(sv);
    CHECK(v10.as<double>() == Approx(19.0).epsilon(1.0e-12));

    nsx::Variant<int, double, std::string> v11(nsx::Variant<int, double>(40));
    CHECK(v11.as<int>() == 40);

    const nsx::Variant<int, double> csv(-1.0);
    nsx::Variant<int, double, std::string> v12(csv);
    CHECK(v12.as<double>() == Approx(-1.0).epsilon(1.0e-12));

    // is

    CHECK(v12.is<double>());
    CHECK(!v12.is<int>());
    CHECK(!v12.is<std::string>());
    CHECK(!v12.is<float>());

    // Constructor from a subset variant
    //    const nsx::Variant<int,double> sv(50.0);
    //    nsx::Variant<int,double,std::string> v6(sv);
    //    NSX_CHECK_CLOSE(v6.as<double>(),50.0,1.0e-12);
}
