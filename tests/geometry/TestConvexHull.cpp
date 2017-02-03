#define BOOST_TEST_MODULE "Test Convex Hull"
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <initializer_list>
#include <iostream>
#include <algorithm>
#include <cstdlib>

#include <nsxlib/kernel/Error.h>
#include <nsxlib/geometry/Face.h>
#include <nsxlib/geometry/ConvexHull.h>

//! Check that the Hull satisfies the convexity condition. This consists in
//! checking that the signed volume between every face and every point is positive.
//! This shows that each point is inside every face and therefore the hull is convex.
template <typename T>
bool CheckConvexity(const SX::Geometry::ConvexHull<T>& chull)
{
    const auto& faces=chull.getFaces();
    const auto& vertices=chull.getVertices();

    for (auto & f: faces)
    {
        for (auto& v : vertices)
        {
            if (f->volumeSign(v) < 0)
                return false;
        }
    }
    return true;
}

using Eigen::Vector3d;
const double tolerance=1e-9;

BOOST_AUTO_TEST_CASE(Test_ConvexHull)
{
    typedef Eigen::Vector3d vector3;

    typedef SX::Geometry::ConvexHull<double> CHullDouble;

    // Create an empty convex hull
    CHullDouble chull;

    // Checks that the hull can not be updated with 0 point to be processed
    BOOST_CHECK_THROW(chull.updateHull(),SX::Kernel::Error<CHullDouble>);

    // Fill it with three vertices to form a tetrahedron
    chull.addVertex(vector3( 0, 0, 0));
    // Checks that the hull can not be updated with only 1 point to be processed
    BOOST_CHECK_THROW(chull.updateHull(),SX::Kernel::Error<CHullDouble>);

    chull.addVertex(vector3(10, 0, 0));
    // Checks that the hull can not be updated with only 2 point to be processed
    BOOST_CHECK_THROW(chull.updateHull(),SX::Kernel::Error<CHullDouble>);

    chull.addVertex(vector3( 0,10, 0));
    // Checks that the hull can not be updated with only 3 point to be processed
    BOOST_CHECK_THROW(chull.updateHull(),SX::Kernel::Error<CHullDouble>);

    chull.addVertex(vector3( 0, 0,10));
    // Checks that with 4 vertices the hull can be built
    BOOST_CHECK_NO_THROW(chull.updateHull());

    const auto& faces=chull.getFaces();
    const auto& edges=chull.getEdges();
    const auto& vertices=chull.getVertices();

    // Check that the number of vertices, edges and faces corresponds to a tetrahedron
    BOOST_CHECK_EQUAL(vertices.size(),4);
    BOOST_CHECK_EQUAL(edges.size(),6);
    BOOST_CHECK_EQUAL(faces.size(),4);

    //! Checks that the hull satisfies the Euler conditions
    BOOST_CHECK(chull.checkEulerConditions());

    //! Checks that the hull satisfies the Convexity condition
    BOOST_CHECK(CheckConvexity(chull));

    // Fill the convex hull with new vertices in order to make a cube
    chull.addVertex(vector3(10,10, 0));
    chull.addVertex(vector3(10, 0,10));
    chull.addVertex(vector3(0, 10,10));
    chull.addVertex(vector3(10,10,10));

    // Update the hull
    chull.updateHull();

    // Check that the number of vertices, edges and faces corresponds to a cube
    BOOST_CHECK_EQUAL(vertices.size(),8);
    BOOST_CHECK_EQUAL(edges.size(),18);
    BOOST_CHECK_EQUAL(faces.size(),12);

    //! Checks that the hull satisfies the Euler conditions
    BOOST_CHECK(chull.checkEulerConditions());

    //! Checks that the hull satisfies the Convexity condition
    BOOST_CHECK(CheckConvexity(chull));

    //! Checks that the volume of the cube is 10*10*10=1000
    BOOST_CHECK_CLOSE(chull.getVolume(),1000,tolerance);

    double oldVolume=chull.getVolume();
    chull.translateToCenter();
    double newVolume=chull.getVolume();
    BOOST_CHECK_CLOSE(oldVolume,newVolume,tolerance);

    // Check that the copy construction is OK
    CHullDouble newhull(chull);
    BOOST_CHECK_EQUAL(chull.getNVertices(),newhull.getNVertices());
    BOOST_CHECK_EQUAL(chull.getNEdges(),newhull.getNEdges());
    BOOST_CHECK_EQUAL(chull.getNFaces(),newhull.getNFaces());
    BOOST_CHECK_CLOSE(chull.getVolume(),newhull.getVolume(),tolerance);

}

