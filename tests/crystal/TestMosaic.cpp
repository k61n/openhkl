#define BOOST_TEST_MODULE "Test Mosaic Class"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>


#include <iostream>
#include <vector>
#include <string>

#include <nsxlib/geometry/Ellipsoid.h>
#include <nsxlib/geometry/ConvexHull.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/instrument/DiffractometerStore.h>
#include <nsxlib/utils/Units.h>
#include <nsxlib/crystal/Mosaic.h>
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>

using namespace nsx;

int run_test()
{

    Sample s("test");

    ConvexHull hull;

    double halfa = 1.0*mm/2.0;
    hull.addVertex(-halfa,-halfa,-halfa);
    hull.addVertex(-halfa, halfa,-halfa);
    hull.addVertex( halfa,-halfa,-halfa);
    hull.addVertex( halfa, halfa,-halfa);
    hull.addVertex(-halfa,-halfa, halfa);
    hull.addVertex(-halfa, halfa, halfa);
    hull.addVertex( halfa,-halfa, halfa);
    hull.addVertex( halfa, halfa, halfa);
    hull.updateHull();

    s.setShape(hull);

    Mosaic mos("D9",0.8409,0.0000901,2.602,0.01);

    DiffractometerStore* ds;
    std::shared_ptr<Diffractometer> diff;
    std::shared_ptr<DataSet> dataf;
    Eigen::MatrixXi v;

    ds = DiffractometerStore::Instance();
    diff = std::shared_ptr<Diffractometer>(ds->buildDiffractometer("D9"));
    dataf = std::shared_ptr<DataSet>(DataReaderFactory().create("hdf", "714898.hdf", diff));

    dataf->open();
    //dataf->readInMemory(nullptr);
    mos.setSample(&s);

    for (int i=1; i<=1; ++i) {
        double newmos = 0.01*static_cast<double>(i);
        mos.setMosaicity(newmos);
        double overlap{0.0};
        std::vector<std::shared_ptr<DataSet>> numors;
        numors.push_back(dataf);
        bool result = mos.run(numors,1e5,overlap);
        BOOST_CHECK(result);
        if (result) {
            std::cout<<newmos<<"  "<< overlap<<std::endl;
        }
    }

    Eigen::Vector3d center1(0,0,0);
    Eigen::Vector3d evals1(2,2,2);
    Eigen::Matrix3d evecs1;
    evecs1 << 1,0,0,0,1,0,0,0,1;

    Ellipsoid ell1(center1,evals1,evecs1);

    Eigen::Vector3d center2(0,0,0);
    Eigen::Vector3d evals2(4,4,4);
    Eigen::Matrix3d evecs2;
    evecs2 << 1,0,0,0,1,0,0,0,1;
    Ellipsoid ell2(center2,evals2,evecs2);

    std::cout<<ellipsoids_overlap(ell1,ell2)<<std::endl;

    return 0;
}

BOOST_AUTO_TEST_CASE(Test_Mosaic)
{
    BOOST_CHECK(run_test() == 0);
}
