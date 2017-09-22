#include <string>
#include <vector>

#include <nsxlib/crystal/Mosaic.h>
#include <nsxlib/data/DataReaderFactory.h>
#include <nsxlib/data/DataSet.h>
#include <nsxlib/geometry/ConvexHull.h>
#include <nsxlib/geometry/Ellipsoid.h>
#include <nsxlib/instrument/Diffractometer.h>
#include <nsxlib/instrument/Sample.h>
#include <nsxlib/utils/NSXTest.h>
#include <nsxlib/utils/Units.h>

int main()
{

    nsx::Sample s("test");

    nsx::ConvexHull hull;

    double halfa = 1.0*nsx::mm/2.0;
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

    nsx::Mosaic mos("D9",0.8409,0.0000901,2.602,0.01);

    nsx::sptrDiffractometer diff;
    nsx::sptrDataSet dataf;
    Eigen::MatrixXi v;

    diff = nsx::Diffractometer::build("D9");
    dataf = nsx::sptrDataSet(nsx::DataReaderFactory().create("hdf", "714898.hdf", diff));

    dataf->open();
    mos.setSample(&s);

    for (int i=1; i<=1; ++i) {
        double newmos = 0.01*static_cast<double>(i);
        mos.setMosaicity(newmos);
        double overlap{0.0};
        nsx::DataList numors;
        numors.push_back(dataf);
        bool result = mos.run(numors,1e5,overlap);
        NSX_CHECK_ASSERT(result);
    }

    Eigen::Vector3d center1(0,0,0);
    Eigen::Vector3d evals1(2,2,2);
    Eigen::Matrix3d evecs1;
    evecs1 << 1,0,0,0,1,0,0,0,1;

    nsx::Ellipsoid ell1(center1,evals1,evecs1);

    Eigen::Vector3d center2(0,0,0);
    Eigen::Vector3d evals2(4,4,4);
    Eigen::Matrix3d evecs2;
    evecs2 << 1,0,0,0,1,0,0,0,1;
    nsx::Ellipsoid ell2(center2,evals2,evecs2);

    return 0;
}
