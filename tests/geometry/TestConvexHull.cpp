#include <fstream>
#include <stdexcept>

#include "core/geometry/AABB.h"
#include "core/geometry/ConvexHull.h"
#include "core/geometry/Face.h"
#include "core/geometry/Vertex.h"

const double tolerance = 1e-9;


//! Check that the Hull satisfies the convexity condition. This consists in
//! checking that the signed volume between every face and every point is
//! positive. This shows that each point is inside every face and therefore the
//! hull is convex.
bool CheckConvexity(const nsx::ConvexHull& chull)
{
    const auto& faces = chull.faces();
    const auto& vertices = chull.vertices();

    for (auto p : faces) {
        nsx::Face* f = p.second;
        for (auto pp : vertices) {
            nsx::Vertex* v = pp.second;
            if (f->volumeSign(v) < 0)
                return false;
        }
    }
    return true;
}
TEST_CASE("test/geometry/TestConvexHull.cpp", "") {

    // Create an empty convex hull
    nsx::ConvexHull chull;

    // Checks that the hull can not be updated with 0 point to be processed
    NSX_CHECK_THROW(chull.updateHull(), std::runtime_error);

    // Fill it with three vertices to form a tetrahedron
    chull.addVertex(Eigen::Vector3d(0, 0, 0));
    // Checks that the hull can not be updated with only 1 point to be processed
    NSX_CHECK_THROW(chull.updateHull(), std::runtime_error);

    chull.addVertex(Eigen::Vector3d(10, 0, 0));
    // Checks that the hull can not be updated with only 2 point to be processed
    NSX_CHECK_THROW(chull.updateHull(), std::runtime_error);

    chull.addVertex(Eigen::Vector3d(0, 10, 0));
    // Checks that the hull can not be updated with only 3 point to be processed
    NSX_CHECK_THROW(chull.updateHull(), std::runtime_error);

    chull.addVertex(Eigen::Vector3d(0, 0, 10));
    // Checks that with 4 vertices the hull can be built
    NSX_CHECK_NO_THROW(chull.updateHull());

    auto faces = chull.faces();
    auto edges = chull.edges();
    auto vertices = chull.vertices();

    // Check that the number of vertices, edges and faces corresponds to a
    // tetrahedron
    NSX_CHECK_EQUAL(vertices.size(), 4);
    NSX_CHECK_EQUAL(edges.size(), 6);
    NSX_CHECK_EQUAL(faces.size(), 4);

    //! Checks that the hull satisfies the Euler conditions
    NSX_CHECK_ASSERT(chull.checkEulerConditions());

    //! Checks that the hull satisfies the Convexity condition
    NSX_CHECK_ASSERT(CheckConvexity(chull));

    // Fill the convex hull with new vertices in order to make a cube
    chull.addVertex(Eigen::Vector3d(10, 10, 0));
    chull.addVertex(Eigen::Vector3d(10, 0, 10));
    chull.addVertex(Eigen::Vector3d(0, 10, 10));
    chull.addVertex(Eigen::Vector3d(10, 10, 10));

    // Update the hull
    chull.updateHull();

    // Check that the number of vertices, edges and faces corresponds to a cube
    NSX_CHECK_EQUAL(chull.nVertices(), 8);
    NSX_CHECK_EQUAL(chull.nEdges(), 18);
    NSX_CHECK_EQUAL(chull.nFaces(), 12);

    //! Checks that the hull satisfies the Euler conditions
    NSX_CHECK_ASSERT(chull.checkEulerConditions());

    //! Checks that the hull satisfies the Convexity condition
    NSX_CHECK_ASSERT(CheckConvexity(chull));

    //! Checks that the volume of the cube is 10*10*10=1000
    NSX_CHECK_CLOSE(chull.volume(), 1000, tolerance);

    double oldVolume = chull.volume();
    chull.translateToCenter();
    double newVolume = chull.volume();
    NSX_CHECK_CLOSE(oldVolume, newVolume, tolerance);

    // Check that the copy construction is OK
    nsx::ConvexHull newhull(chull);
    NSX_CHECK_EQUAL(chull.nVertices(), newhull.nVertices());
    NSX_CHECK_EQUAL(chull.nEdges(), newhull.nEdges());
    NSX_CHECK_EQUAL(chull.nFaces(), newhull.nFaces());
    NSX_CHECK_CLOSE(chull.volume(), newhull.volume(), tolerance);

    nsx::ConvexHull box;
    box.addVertex(Eigen::Vector3d(0, 0, 0));
    box.addVertex(Eigen::Vector3d(0, 0, 1));
    box.addVertex(Eigen::Vector3d(0, 1, 0));
    box.addVertex(Eigen::Vector3d(0, 1, 1));
    box.addVertex(Eigen::Vector3d(1, 0, 0));
    box.addVertex(Eigen::Vector3d(1, 0, 1));
    box.addVertex(Eigen::Vector3d(1, 1, 0));
    box.addVertex(Eigen::Vector3d(1, 1, 1));
    box.updateHull();

    nsx::AABB bb = box.aabb();

    NSX_CHECK_LOWER_THAN((bb.lower() - Eigen::Vector3d(0, 0, 0)).norm(), 1e-9);
    NSX_CHECK_LOWER_THAN((bb.upper() - Eigen::Vector3d(1, 1, 1)).norm(), 1e-9);

    Eigen::Vector3d p = {0.5, 0.5, 0.5};
    NSX_CHECK_EQUAL(bb.isInside(p), box.contains(p));
    p = {-0.5, 0.5, 0.5};
    NSX_CHECK_EQUAL(bb.isInside(p), box.contains(p));
    p = {0.5, -0.5, 0.5};
    NSX_CHECK_EQUAL(bb.isInside(p), box.contains(p));
    p = {0.5, 0.5, -0.5};
    NSX_CHECK_EQUAL(bb.isInside(p), box.contains(p));

    chull.reset();

    std::ifstream f_in("convex_hull_vertices.xyz");

    size_t n_points;

    f_in >> n_points;

    for (size_t i = 0; i < n_points; ++i) {
        double x, y, z;
        f_in >> x >> y >> z;
        Eigen::Vector3d vertex(x, y, z);
        chull.addVertex(vertex);
    }

    f_in.close();

    chull.updateHull();

    // Check the results with the one obtained by third-party code
    // http://cs.smith.edu/~jorourke/

    f_in.open("convex_hull_faces.xyz");

    size_t n_triangles;

    f_in >> n_triangles;

    using triangle = std::vector<Eigen::Vector3d>;
    std::vector<triangle> triangles;
    triangles.reserve(n_triangles);
    for (size_t i = 0; i < n_triangles; ++i) {
        triangle t;
        for (size_t j = 0; j < 3; ++j) {
            double x, y, z;
            f_in >> x >> y >> z;
            t.emplace_back(x, y, z);
        }
        triangles.push_back(t);
    }

    f_in.close();

    faces = chull.faces();

    NSX_CHECK_EQUAL(faces.size(), triangles.size());

    size_t comp(0);
    for (auto p : faces) {
        nsx::Face* f = p.second;
        const auto& t = triangles[comp];
        for (size_t i = 0; i < 3; ++i) {
            nsx::Vertex* v = f->_vertices[i];
            NSX_CHECK_CLOSE(v->_coords[0], t[i](0), tolerance);
            NSX_CHECK_CLOSE(v->_coords[1], t[i](1), tolerance);
            NSX_CHECK_CLOSE(v->_coords[2], t[i](2), tolerance);
        }
        ++comp;
    }

    return 0;
}
