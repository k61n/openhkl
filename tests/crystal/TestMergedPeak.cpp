#include <map>
#include <string>
#include <iostream>

#include <Eigen/Core>

#include <nsxlib/crystal/SpaceGroup.h>
#include <nsxlib/crystal/UnitCell.h>
#include <nsxlib/crystal/GruberReduction.h>
#include <nsxlib/crystal/Peak3D.h>
#include <nsxlib/data/MergedData.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/utils/NSXTest.h>

using namespace nsx;

#pragma message "This test is not complete"

int main()
{
    nsx::DataReaderFactory factory;    
    nsx::sptrExperiment expt(new nsx::Experiment("test", "BioDiff2500"));
    auto diff = expt->getDiffractometer();
    nsx::sptrDataSet dataf(factory.create("hdf", "gal3.hdf", diff));    
    expt->addData(dataf);

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
    NSX_CHECK_EQUAL(cell.getBravaisTypeSymbol(), std::string("oP"));

    cell.setSpaceGroup(group.symbol());

    auto reflections = cell.generateReflectionsInShell(2.1, 50.0, 2.665);

    #if 0
    auto peaks = dataf->hasPeaks(reflections, cell.reciprocalBasis());
    
    NSX_CHECK_EQUAL(reflections.size(), static_cast<size_t>(107460));

    MergedData data1(group, true), data2(group, false);

    for (auto&& peak: peaks) {
        data1.addPeak(peak);
        data2.addPeak(peak);

        auto hkl = peak->getIntegerMillerIndices();
        NSX_CHECK_EQUAL(group.isExtinct(hkl(0), hkl(1), hkl(2)), false);
    }

    auto peaks1 = data1.getPeaks();
    auto peaks2 = data2.getPeaks();

    const size_t group_size = group.groupElements().size();

    // todo: we need to fix this after the change PeakCalc -> Peak3D
    #if 0
    NSX_CHECK_EQUAL(peaks1.size(), 14493);
    NSX_CHECK_EQUAL(peaks2.size(), 26897);
    #endif

    int count1 = 0, count2 = 0;

    for (auto&& peak: peaks1) {

        auto p = *peak.getPeaks().begin();

        if (p->getIntegerMillerIndices().squaredNorm() < 1) {
            continue;
        }

        ++count1;

        NSX_CHECK_EQUAL(group_size*2, peak.getPeaks().size());

        if (group_size*2 != peak.getPeaks().size()) {
            std::cout << "FAILURE:" << std::endl;
            for (auto&& p: peak.getPeaks()) {   
                std::cout << p->getIntegerMillerIndices() << std::endl;
            }
        }
    }

    for (auto&& peak: peaks2) {

        auto p = *peak.getPeaks().begin();
        
        if (p->getIntegerMillerIndices().squaredNorm() < 1) {
            continue;
        }

        ++count2;

        NSX_CHECK_EQUAL(group_size, peak.getPeaks().size());

        if (group_size != peak.getPeaks().size()) {
            std::cout << "FAILURE:" << std::endl;
            for (auto&& p: peak.getPeaks()) {             
                std::cout << p->getIntegerMillerIndices() << std::endl;
            }
        }
    }

    NSX_CHECK_EQUAL(2*count1, count2);

    // todo: fix this after removal of PeakCalc class
    #if 0
    for (auto&& peak: peaks2) {
        auto hkl = peak.getIndex();
        PeakCalc p(hkl(0), hkl(1), hkl(2), 0, 0, 0);
        PeakCalc q(-hkl(0), -hkl(1), -hkl(2), 0, 0, 0);
        MergedPeak m1(group, true), m2(group, false);
        m1.addPeak(p);
        m2.addPeak(q);
        auto it = peaks1.find(peak);
        NSX_CHECK_ASSERT((peaks1.find(m1) != peaks1.end()) || (peaks1.find(m2) != peaks1.end()));
    }

    for (auto&& peak: peaks1) {
        auto hkl = peak.getIndex();
        PeakCalc p(hkl(0), hkl(1), hkl(2), 0, 0, 0);
        MergedPeak m1(group, true);
        m1.addPeak(p);
        NSX_CHECK_ASSERT(peaks2.find(m1) != peaks2.end());
    }
    #endif

    #endif

    return 0;
}
