#include <stdexcept>

#include <nsxlib/geometry/ConvexHull.h>
#include <nsxlib/geometry/Face.h>
#include <nsxlib/utils/NSXTest.h>

const double tolerance=1e-9;

//! Check that the Hull satisfies the convexity condition. This consists in
//! checking that the signed volume between every face and every point is positive.
//! This shows that each point is inside every face and therefore the hull is convex.
bool CheckConvexity(const nsx::ConvexHull& chull)
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

int main()
{
    // Create an empty convex hull
    nsx::ConvexHull chull;

    // Checks that the hull can not be updated with 0 point to be processed
    NSX_CHECK_THROW(chull.updateHull(),std::runtime_error);

    // Fill it with three vertices to form a tetrahedron
    chull.addVertex(Eigen::Vector3d( 0, 0, 0));
    // Checks that the hull can not be updated with only 1 point to be processed
    NSX_CHECK_THROW(chull.updateHull(),std::runtime_error);

    chull.addVertex(Eigen::Vector3d(10, 0, 0));
    // Checks that the hull can not be updated with only 2 point to be processed
    NSX_CHECK_THROW(chull.updateHull(),std::runtime_error);

    chull.addVertex(Eigen::Vector3d( 0,10, 0));
    // Checks that the hull can not be updated with only 3 point to be processed
    NSX_CHECK_THROW(chull.updateHull(),std::runtime_error);

    chull.addVertex(Eigen::Vector3d( 0, 0,10));
    // Checks that with 4 vertices the hull can be built
    NSX_CHECK_NO_THROW(chull.updateHull());

    const auto& faces=chull.getFaces();
    const auto& edges=chull.getEdges();
    const auto& vertices=chull.getVertices();

    // Check that the number of vertices, edges and faces corresponds to a tetrahedron
    NSX_CHECK_EQUAL(vertices.size(),4);
    NSX_CHECK_EQUAL(edges.size(),6);
    NSX_CHECK_EQUAL(faces.size(),4);

    //! Checks that the hull satisfies the Euler conditions
    NSX_CHECK_ASSERT(chull.checkEulerConditions());

    //! Checks that the hull satisfies the Convexity condition
    NSX_CHECK_ASSERT(CheckConvexity(chull));

    // Fill the convex hull with new vertices in order to make a cube
    chull.addVertex(Eigen::Vector3d(10,10, 0));
    chull.addVertex(Eigen::Vector3d(10, 0,10));
    chull.addVertex(Eigen::Vector3d(0, 10,10));
    chull.addVertex(Eigen::Vector3d(10,10,10));

    // Update the hull
    chull.updateHull();

    // Check that the number of vertices, edges and faces corresponds to a cube
    NSX_CHECK_EQUAL(vertices.size(),8);
    NSX_CHECK_EQUAL(edges.size(),18);
    NSX_CHECK_EQUAL(faces.size(),12);

    //! Checks that the hull satisfies the Euler conditions
    NSX_CHECK_ASSERT(chull.checkEulerConditions());

    //! Checks that the hull satisfies the Convexity condition
    NSX_CHECK_ASSERT(CheckConvexity(chull));

    //! Checks that the volume of the cube is 10*10*10=1000
    NSX_CHECK_CLOSE(chull.getVolume(),1000,tolerance);

    double oldVolume=chull.getVolume();
    chull.translateToCenter();
    double newVolume=chull.getVolume();
    NSX_CHECK_CLOSE(oldVolume,newVolume,tolerance);

    // Check that the copy construction is OK
    nsx::ConvexHull newhull(chull);
    NSX_CHECK_EQUAL(chull.getNVertices(),newhull.getNVertices());
    NSX_CHECK_EQUAL(chull.getNEdges(),newhull.getNEdges());
    NSX_CHECK_EQUAL(chull.getNFaces(),newhull.getNFaces());
    NSX_CHECK_CLOSE(chull.getVolume(),newhull.getVolume(),tolerance);

    return 0;
}

