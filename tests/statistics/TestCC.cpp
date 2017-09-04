#define BOOST_TEST_MODULE "Test CC"
#define BOOST_TEST_DYN_LINK

#include <map>
#include <string>
#include <random>

#include <Eigen/Core>

#include <boost/test/unit_test.hpp>

#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/crystal/GruberReduction.h>
#include <nsxlib/data/MergedData.h>
#include <nsxlib/statistics/CC.h>

using namespace nsx;

BOOST_AUTO_TEST_CASE(Test_CC)
{
    const double deg = M_PI / 180.0;

    const double a = 46.3559;
    const double b = 59.9255;
    const double c = 85.5735;

    const double alpha = 90.0 * deg;
    const double beta = 90.0 * deg;
    const double gamma = 90.0 * deg;

    const double gruber_tolerance = 1e-2;

    const double mu_I = 1e6;
    const double sigma_I = 1e4;
    const double sigma_delta = 1e2;

    std::random_device rd;
    std::mt19937 gen(rd());
 
    // values near the mean are the most likely
    // standard deviation affects the dispersion of generated values from the mean
    std::normal_distribution<> intensity_dist(mu_I, sigma_I);
    std::normal_distribution<> error_dist(0.0, sigma_delta);

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
    
    MergedData data0(group, true), data1(group, true), data2(group, true); 

    for (auto&& hkl: reflections) {
        PeakCalc p(hkl(0), hkl(1), hkl(2), 0.0, 0.0, 0.0);
        p._intensity = Intensity(intensity_dist(gen));
        data0.addPeak(p);
    }

    double I_sum = 0.0;
    double I2_sum = 0.0;
    const double n = group.groupElements().size() * 2.0;
    unsigned int num_raw_peaks = 0;

    for(auto&& peak: data0.getPeaks()) {
        auto hkl = peak.getIndex();
        PeakCalc p0(hkl(0), hkl(1), hkl(2), 0, 0, 0);
        PeakCalc p1(p0), p2(p0);

        const double intensity = peak.getIntensity().getValue();

        for (auto&& raw_peak: peak.getPeaks()) {
            p1._intensity = intensity + error_dist(gen);
            p2._intensity = intensity_dist(gen);
            data1.addPeak(p1);
            data2.addPeak(p2);
            ++num_raw_peaks;
        }

        I_sum += intensity;
        I2_sum += std::pow(intensity, 2);
    }

    const double N = data0.getPeaks().size();
    const double redundancy = num_raw_peaks / N;
    const double sigma_J = sigma_I / std::sqrt(redundancy);
    const double sigma_eps = sigma_delta / std::sqrt(redundancy * 0.5);

    BOOST_CHECK_CLOSE(redundancy, n, 10.0);

    const double expected_cc_half = sigma_J*sigma_J / (sigma_J*sigma_J + sigma_eps*sigma_eps);
    const double expected_cc_true = sigma_J / std::sqrt(sigma_J*sigma_J + 0.5*sigma_eps*sigma_eps);

    CC cc1, cc2;
    cc1.calculate(data1);
    cc2.calculate(data2);

    BOOST_CHECK_EQUAL(cc1.nPeaks(), cc2.nPeaks());

    BOOST_CHECK_CLOSE(cc1.CChalf(), expected_cc_half, 1e-1);
    BOOST_CHECK_CLOSE(cc1.CCstar(), expected_cc_true, 1e-1);

    BOOST_CHECK_SMALL(cc2.CChalf(), 4.0 / std::sqrt(cc2.nPeaks()));

}
