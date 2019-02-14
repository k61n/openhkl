#include <string>

#include <core/NSXTest.h>
#include <core/Variant.h>

NSX_INIT_TEST

int main() {

  // Default constructor
  nsx::Variant<int, double, std::string> v1;
  NSX_CHECK_EQUAL(v1.as<int>(), 0);

  // Constructor from values

  nsx::Variant<int, double, std::string> v2(20.0);
  NSX_CHECK_CLOSE(v2.as<double>(), 20.0, 1.0e-12);

  nsx::Variant<int, double, std::string> v3(std::string("hello"));
  NSX_CHECK_ASSERT(v3.as<std::string>().compare("hello") == 0);

  const int ci(50);
  nsx::Variant<int, double, std::string> v4(ci);
  NSX_CHECK_EQUAL(v4.as<int>(), 50);

  double d(-25.0);
  nsx::Variant<int, double, std::string> v5(d);
  NSX_CHECK_CLOSE(v5.as<double>(), -25.0, 1.0e-12);

  // Move/Copy constructors from another Variant

  nsx::Variant<int, double, std::string> v6(v1);
  NSX_CHECK_EQUAL(v6.as<int>(), 0);

  nsx::Variant<int, double, std::string> v7(
      nsx::Variant<int, double, std::string>(40));
  NSX_CHECK_EQUAL(v7.as<int>(), 40);

  const nsx::Variant<int, double, std::string> cv(17.0);
  nsx::Variant<int, double, std::string> v8(cv);
  NSX_CHECK_CLOSE(v8.as<double>(), 17.0, 1.0e-12);

  nsx::Variant<int, double, std::string> v9(
      nsx::Variant<int, double, std::string>(19));
  NSX_CHECK_EQUAL(v9.as<int>(), 19);

  // Move/Copy constructors from a subset Variant

  nsx::Variant<int, double> sv(19.0);
  nsx::Variant<int, double, std::string> v10(sv);
  NSX_CHECK_CLOSE(v10.as<double>(), 19.0, 1.0e-12);

  nsx::Variant<int, double, std::string> v11(nsx::Variant<int, double>(40));
  NSX_CHECK_EQUAL(v11.as<int>(), 40);

  const nsx::Variant<int, double> csv(-1.0);
  nsx::Variant<int, double, std::string> v12(csv);
  NSX_CHECK_CLOSE(v12.as<double>(), -1.0, 1.0e-12);

  // is

  NSX_CHECK_ASSERT(v12.is<double>());
  NSX_CHECK_ASSERT(!v12.is<int>());
  NSX_CHECK_ASSERT(!v12.is<std::string>());
  NSX_CHECK_ASSERT(!v12.is<float>());

  // Constructor from a subset variant
  //    const nsx::Variant<int,double> sv(50.0);
  //    nsx::Variant<int,double,std::string> v6(sv);
  //    NSX_CHECK_CLOSE(v6.as<double>(),50.0,1.0e-12);

  return 0;
}
