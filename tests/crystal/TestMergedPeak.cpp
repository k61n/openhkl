#define BOOST_TEST_MODULE "Test Merged Peak"
#define BOOST_TEST_DYN_LINK

#include <map>
#include <string>
#include <iostream>

#include <Eigen/Core>

#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/crystal/GruberReduction.h>
#include <nsxlib/data/MergedData.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_MergedPeak)
{
    const double deg = M_PI / 180.0;

    const double a = 46.3559;
    const double b = 59.9255;
    const double c = 85.5735;

    const double alpha = 90.0 * deg;
    const double beta = 90.0 * deg;
    const double gamma = 90.0 * deg;

    const double gruber_tolerance = 1e-2;

    //SpaceGroup group("P 21 21 21");
    SpaceGroup group("P 21 21 2");
    UnitCell cell(a, b, c, alpha, beta, gamma);
 
    Eigen::Matrix3d G = cell.metric();

    Eigen::Matrix3d P;

    GruberReduction gruber(G, gruber_tolerance);
    BravaisType bravais_type;
    LatticeCentring centering;

    int match = gruber.reduce(P, centering, bravais_type);
    cell.setBravaisType(bravais_type);
    cell.setLatticeCentring(centering);
    cell.transform(P);
    BOOST_CHECK_EQUAL(cell.getBravaisTypeSymbol(), std::string("oP"));

    cell.setSpaceGroup(group.symbol());

    auto reflections = cell.generateReflectionsInShell(2.1, 50.0, 2.665);
    
    BOOST_CHECK_EQUAL(reflections.size(), 107460);

    MergedData data1(group, true), data2(group, false);

    for (auto&& hkl: reflections) {
        // ...
        PeakCalc p(hkl(0), hkl(1), hkl(2), 0.0, 0.0, 0.0);
        data1.addPeak(p);
        data2.addPeak(p);

        BOOST_CHECK_EQUAL(group.isExtinct(p._h, p._k, p._l), false);
    }

    auto peaks1 = data1.getPeaks();
    auto peaks2 = data2.getPeaks();

    const int group_size = group.groupElements().size();

    BOOST_CHECK_EQUAL(peaks1.size(), 14493);
    BOOST_CHECK_EQUAL(peaks2.size(), 26897);

    int count1 = 0, count2 = 0;

    for (auto&& peak: peaks1) {

        const PeakCalc& p = *peak.getPeaks().begin();

        if (p._h * p._k * p._l == 0) {
            continue;
        }

        ++count1;

        BOOST_CHECK_EQUAL(group_size*2, peak.getPeaks().size());

        if (group_size*2 != peak.getPeaks().size()) {
            std::cout << "FAILURE:" << std::endl;
            for (auto&& p: peak.getPeaks()) {             
                std::cout << p._h << " " << p._k << " " << p._l << std::endl;
            }
        }
    }

    for (auto&& peak: peaks2) {

        const PeakCalc& p = *peak.getPeaks().begin();

        if (p._h * p._k * p._l == 0) {
            continue;
        }

        ++count2;

        BOOST_CHECK_EQUAL(group_size, peak.getPeaks().size());

        if (group_size != peak.getPeaks().size()) {
            std::cout << "FAILURE:" << std::endl;
            for (auto&& p: peak.getPeaks()) {             
                std::cout << p._h << " " << p._k << " " << p._l << std::endl;
            }
        }
    }

    BOOST_CHECK_EQUAL(2*count1, count2);

    for (auto&& peak: peaks2) {
        auto hkl = peak.getIndex();
        PeakCalc p(hkl(0), hkl(1), hkl(2), 0, 0, 0);
        PeakCalc q(-hkl(0), -hkl(1), -hkl(2), 0, 0, 0);
        MergedPeak m1(group, true), m2(group, false);
        m1.addPeak(p);
        m2.addPeak(q);
        auto it = peaks1.find(peak);
        BOOST_CHECK((peaks1.find(m1) != peaks1.end()) || (peaks1.find(m2) != peaks1.end()));
    }

    for (auto&& peak: peaks1) {
        auto hkl = peak.getIndex();
        PeakCalc p(hkl(0), hkl(1), hkl(2), 0, 0, 0);
        MergedPeak m1(group, true);
        m1.addPeak(p);
        BOOST_CHECK(peaks2.find(m1) != peaks2.end());
    }
}
