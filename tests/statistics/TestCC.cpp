#include <random>
#include <string>

#include <Eigen/Dense>

#include <nsxlib/CC.h>
#include <nsxlib/ComponentState.h>
#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>

#include <nsxlib/Experiment.h>
#include <nsxlib/GruberReduction.h>
#include <nsxlib/MergedData.h>
#include <nsxlib/MillerIndex.h>
#include <nsxlib/NSXTest.h>
#include <nsxlib/Peak3D.h>
#include <nsxlib/PeakPredictor.h>
#include <nsxlib/ShapeLibrary.h>
#include <nsxlib/SpaceGroup.h>
#include <nsxlib/UnitCell.h>

using namespace nsx;

int main()
{
    nsx::DataReaderFactory factory;    
    nsx::sptrExperiment expt(new nsx::Experiment("test", "BioDiff2500"));
    auto diff = expt->getDiffractometer();
    nsx::sptrDataSet dataf(factory.create("hdf", "blank.hdf", diff));    
    expt->addData(dataf);

    const double deg = M_PI / 180.0;

    const double a = 46.3559;
    const double b = 59.9255;
    const double c = 85.5735;

    const double alpha = 90.0 * deg;
    const double beta = 90.0 * deg;
    const double gamma = 90.0 * deg;

    const double gruber_tolerance = 1e-2;

    const double mu_I = 1e7;
    const double sigma_I = mu_I / 10.0;
    const double sigma_delta = sigma_I / 10.0;

    std::random_device rd;
    std::mt19937 gen(rd());
 
    // values near the mean are the most likely
    // standard deviation affects the dispersion of generated values from the mean
    std::normal_distribution<> intensity_dist(mu_I, sigma_I);
    std::normal_distribution<> error_dist(0.0, sigma_delta);

    //SpaceGroup group("P 21 21 21");
    SpaceGroup group("P 21 21 2");
    nsx::sptrUnitCell cell(new nsx::UnitCell(a, b, c, alpha, beta, gamma));
 
    Eigen::Matrix3d G = cell->metric();

    Eigen::Matrix3d P;

    GruberReduction gruber(G, gruber_tolerance);
    BravaisType bravais_type;
    LatticeCentring centering;

    gruber.reduce(P, centering, bravais_type);
    cell->setBravaisType(bravais_type);
    cell->setLatticeCentring(centering);
    cell->transform(P);
    NSX_CHECK_EQUAL(cell->getBravaisTypeSymbol(), std::string("oP"));

    cell->setSpaceGroup(group.symbol());

    nsx::PeakPredictor pred(cell, nsx::ShapeLibrary(), 2.0, 5.0, 0);
    auto peaks = pred.predict(dataf);  

    MergedData data0(group, true), data1(group, true);

    for (auto p: peaks) {
        p->setRawIntensity(Intensity(error_dist(gen)));
        p->addUnitCell(cell, true);
        data0.addPeak(p);
    }

    const double n = group.groupElements().size() * 2.0;
    unsigned int num_raw_peaks = 0;

    for(auto&& peak: data0.getPeaks()) {
        const double intensity = intensity_dist(gen);

        for (auto&& raw_peak: peak.getPeaks()) {
            nsx::sptrPeak3D p1 = nsx::sptrPeak3D(new nsx::Peak3D(*raw_peak));
            double i1 = intensity + error_dist(gen);
            p1->setRawIntensity(i1);
            data1.addPeak(p1);
            ++num_raw_peaks;
        }
    }

    double J_sum = 0.0;
    double J2_sum = 0.0;

    for (auto&& peak: data1.getPeaks()) {
        const double i = peak.getIntensity().value();
        J_sum += i;
        J2_sum += i*i;
    }
   
    NSX_CHECK_EQUAL(data0.getPeaks().size(), data1.getPeaks().size());

    const double N = data0.getPeaks().size();
    const double mu_J = J_sum / N;
    const double sigma_J = std::sqrt((J2_sum - N*mu_J*mu_J) / (N-1.0));
    const double redundancy = num_raw_peaks / N;

    const double sigma_eps = sigma_delta / std::sqrt(redundancy * 0.5);

    // check that redundancy is approximately correct (depends on detector coverage)
    NSX_CHECK_CLOSE(redundancy, n, 10.0);

    const double expected_cc_half = sigma_J*sigma_J / (sigma_J*sigma_J + sigma_eps*sigma_eps);
    const double expected_cc_true = sigma_J / std::sqrt(sigma_J*sigma_J + 0.5*sigma_eps*sigma_eps);

    CC cc0, cc1;
    cc0.calculate(data0);
    cc1.calculate(data1);

    NSX_CHECK_EQUAL(cc0.nPeaks(), cc1.nPeaks());

    // data0 is totally random, so CC should be very close to zero
    NSX_CHECK_SMALL(cc0.CChalf(), 4.0 / std::sqrt(cc0.nPeaks()));

    // data1 should have large CC, but not identically 1.0. This is a sanity check
    NSX_CHECK_ASSERT(cc1.CChalf() < 1.0);
    NSX_CHECK_ASSERT(cc1.CCstar() < 1.0);

    // check that the CC values are close to the ones expected from theory, using the known 
    // statistic used to generate the fake data
    NSX_CHECK_CLOSE(cc1.CChalf(), expected_cc_half, 0.05);
    NSX_CHECK_CLOSE(cc1.CCstar(), expected_cc_true, 0.05);

    return 0;
}
