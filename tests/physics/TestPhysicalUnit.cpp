#include <stdexcept>
#include <string>

#include <nsxlib/NSXTest.h>
#include <nsxlib/PhysicalUnit.h>

const double largeTolerance=1.0e-2;
const double smallTolerance=1.0e-9;

int main()
{
    // unit conversion tested according value got from
    // http://www.translatorscafe.com/cafe/EN/units-converter/energy/1-65/joule-kelvin/

    // Check that a bad input unit actually throws
    NSX_CHECK_THROW(nsx::PhysicalUnit u(1.0,"xxxxxx"),std::runtime_error);

    nsx::PhysicalUnit u1(1.0,"km/s");

    // Check that an invalid conversion throws
    NSX_CHECK_THROW(u1.convert("km*K"),std::runtime_error);

    // Check a valid conversion
    NSX_CHECK_CLOSE(u1.convert("m/s"), 1.0e+3, smallTolerance);
    NSX_CHECK_CLOSE(u1.convertToSI(), 1.0e+3, smallTolerance);

    // Change the value
    u1.setValue(5.0);

    // Check again a valid conversion
    NSX_CHECK_CLOSE(u1.convert("m/s"), 5.0e+3, smallTolerance);
    NSX_CHECK_CLOSE(u1.convertToSI(), 5.0e+3, smallTolerance);

    nsx::PhysicalUnit u2(5.0,"J");
    NSX_CHECK_CLOSE(u2.convert("keV"), 3.121e+16, largeTolerance);
    NSX_CHECK_CLOSE(u2.convert("kg"), 5.56325028e-17, largeTolerance);
    // Check that converting J to cd throws
    NSX_CHECK_THROW(u2.convert("cd"),std::runtime_error);

    nsx::PhysicalUnit u3(5.0,"eV");
    NSX_CHECK_CLOSE(u3.convert("g"), 8.913313479732e-33, largeTolerance);

    nsx::PhysicalUnit u4(5.0,"g");
    NSX_CHECK_CLOSE(u4.convert("eV"), 5.0*5.60958616722e+32, largeTolerance);

    nsx::PhysicalUnit u5(3.0,"J**2");
    NSX_CHECK_CLOSE(u5.convert("keV**2"), 3.0*6.241506363094e+15*6.241506363094e+15, largeTolerance);
    NSX_CHECK_CLOSE(u5.convert("g**2"), 3.0*1.112650056e-14*1.112650056e-14, largeTolerance);

    nsx::PhysicalUnit u6(5.0,"meV**2");
    NSX_CHECK_CLOSE(u6.convert("g**2"), 5*1.782662695946e-36*1.782662695946e-36, largeTolerance);
}
