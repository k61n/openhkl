#include "test/cpp/catch.hpp"
#include <fstream>
#include <stdexcept>

#include "core/geometry/AABB.h"
#include "core/hull/ConvexHull.h"
#include "core/hull/Face.h"
#include "core/hull/Vertex.h"

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

TEST_CASE("test/geometry/TestConvexHull.cpp", "")
{

    // Create an empty convex hull
    nsx::ConvexHull chull;

    // Checks that the hull can not be updated with 0 point to be processed
    CHECK_THROWS_AS(chull.updateHull(), std::runtime_error);

    // Fill it with three vertices to form a tetrahedron
    chull.addVertex(Eigen::Vector3d(0, 0, 0));
    // Checks that the hull can not be updated with only 1 point to be processed
    CHECK_THROWS_AS(chull.updateHull(), std::runtime_error);

    chull.addVertex(Eigen::Vector3d(10, 0, 0));
    // Checks that the hull can not be updated with only 2 point to be processed
    CHECK_THROWS_AS(chull.updateHull(), std::runtime_error);

    chull.addVertex(Eigen::Vector3d(0, 10, 0));
    // Checks that the hull can not be updated with only 3 point to be processed
    CHECK_THROWS_AS(chull.updateHull(), std::runtime_error);

    chull.addVertex(Eigen::Vector3d(0, 0, 10));
    // Checks that with 4 vertices the hull can be built
    CHECK_NOTHROW(chull.updateHull());

    auto faces = chull.faces();
    auto edges = chull.edges();
    auto vertices = chull.vertices();

    // Check that the number of vertices, edges and faces corresponds to a
    // tetrahedron
    CHECK(vertices.size() == 4);
    CHECK(edges.size() == 6);
    CHECK(faces.size() == 4);

    //! Checks that the hull satisfies the Euler conditions
    CHECK(chull.checkEulerConditions());

    //! Checks that the hull satisfies the Convexity condition
    CHECK(CheckConvexity(chull));

    // Fill the convex hull with new vertices in order to make a cube
    chull.addVertex(Eigen::Vector3d(10, 10, 0));
    chull.addVertex(Eigen::Vector3d(10, 0, 10));
    chull.addVertex(Eigen::Vector3d(0, 10, 10));
    chull.addVertex(Eigen::Vector3d(10, 10, 10));

    // Update the hull
    chull.updateHull();

    // Check that the number of vertices, edges and faces corresponds to a cube
    CHECK(chull.nVertices() == 8);
    CHECK(chull.nEdges() == 18);
    CHECK(chull.nFaces() == 12);

    //! Checks that the hull satisfies the Euler conditions
    CHECK(chull.checkEulerConditions());

    //! Checks that the hull satisfies the Convexity condition
    CHECK(CheckConvexity(chull));

    //! Checks that the volume of the cube is 10*10*10=1000
    CHECK(chull.volume() == Approx(1000).epsilon(tolerance));

    double oldVolume = chull.volume();
    chull.translateToCenter();
    double newVolume = chull.volume();
    CHECK(oldVolume == Approx(newVolume).epsilon(tolerance));

    // Check that the copy construction is OK
    nsx::ConvexHull newhull(chull);
    CHECK(chull.nVertices() == newhull.nVertices());
    CHECK(chull.nEdges() == newhull.nEdges());
    CHECK(chull.nFaces() == newhull.nFaces());
    CHECK(chull.volume() == Approx(newhull.volume()).epsilon(tolerance));

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

    CHECK((bb.lower() - Eigen::Vector3d(0, 0, 0)).norm() < 1e-9);
    CHECK((bb.upper() - Eigen::Vector3d(1, 1, 1)).norm() < 1e-9);

    Eigen::Vector3d p = {0.5, 0.5, 0.5};
    CHECK(bb.isInside(p) == box.contains(p));
    p = {-0.5, 0.5, 0.5};
    CHECK(bb.isInside(p) == box.contains(p));
    p = {0.5, -0.5, 0.5};
    CHECK(bb.isInside(p) == box.contains(p));
    p = {0.5, 0.5, -0.5};
    CHECK(bb.isInside(p) == box.contains(p));

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

    CHECK(faces.size() == triangles.size());

    size_t comp(0);
    for (auto p : faces) {
        nsx::Face* f = p.second;
        const auto& t = triangles[comp];
        for (size_t i = 0; i < 3; ++i) {
            nsx::Vertex* v = f->_vertices[i];
            CHECK(v->_coords[0] == Approx(t[i](0)).epsilon(tolerance));
            CHECK(v->_coords[1] == Approx(t[i](1)).epsilon(tolerance));
            CHECK(v->_coords[2] == Approx(t[i](2)).epsilon(tolerance));
        }
        ++comp;
    }
}
