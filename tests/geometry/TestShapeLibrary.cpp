#include <Eigen/Dense>

#include <nsxlib/MillerIndex.h>
#include <nsxlib/ShapeLibrary.h>
#include <nsxlib/NSXTest.h>

using nsx::ShapeLibrary;
using nsx::MillerIndex;
using Eigen::Matrix3d;

int main()
{
    ShapeLibrary library;

    Matrix3d A, B;
    A.setIdentity();

    library.addShape(MillerIndex(0, 0, 0), A);
    library.addShape(MillerIndex(0, 0, 1), 2*A);
    library.addShape(MillerIndex(0, 2, 0), 3*A);
    library.addShape(MillerIndex(-3, 0, 0), 4*A);

    // look up (0,0,0)
    B = library.predict(MillerIndex(0,0,0), 2);
    NSX_CHECK_SMALL((A-B).norm(), 1e-9);

    // look up (0,0,1)
    B = library.predict(MillerIndex(0,0,1), 2);
    NSX_CHECK_SMALL((2*A-B).norm(), 1e-9);

    // look up (0,2,0)
    B = library.predict(MillerIndex(0,2,0), 2);
    NSX_CHECK_SMALL((3*A-B).norm(), 1e-9);

    // look up (-3,0,0)
    B = library.predict(MillerIndex(-3,0,0), 2);
    NSX_CHECK_SMALL((4*A-B).norm(), 1e-9);

    // predict (0, 1, 0)
    B = library.predict(MillerIndex(0,1,0), 2);
    NSX_CHECK_SMALL((2*A-B).norm(), 1e-9);

    // predict (-1, 1, 0)
    B = library.predict(MillerIndex(-1,1,0), 2);
    NSX_CHECK_SMALL((10.0/4.0*A-B).norm(), 1e-9);

    // check that default shape is returned for indices with no neighbors
    library.setDefaultShape(100*A);
    B = library.predict(MillerIndex(10,10,0), 2);
    NSX_CHECK_SMALL((100*A-B).norm(), 1e-9);

    // check that the average shape is returned for indices with more than one shape available
    library.addShape(MillerIndex(0, 0, 0), 2*A);
    B = library.predict(MillerIndex(0,0,0), 2);
    NSX_CHECK_SMALL((1.5*A-B).norm(), 1e-9);

    return 0;
}
