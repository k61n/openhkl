#define BOOST_TEST_MODULE "Test NDTree"
#define BOOST_TEST_DYN_LINK

#include <cmath>
#include <random>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/AABB.h>
#include <nsxlib/geometry/NDTree.h>
#include <nsxlib/geometry/Ellipsoid.h>

using namespace SX::Geometry;
using Eigen::Vector3d;

const double tolerance=1e-5;
using uint = unsigned int;

void collision_test()
{
    using Ellipsoid3D = SX::Geometry::Ellipsoid<double, 3>;

    NDTree<double,3> tree({0,0,0},{100,100,100});
    auto vects = Eigen::Matrix3d::Identity();
    auto vals = Eigen::Vector3d(0.45, 0.45, 0.45);
    auto center = Eigen::Vector3d(0.0, 0.0, 0.0);

    std::vector<Ellipsoid3D*> shapes;

    // lattice of non-intersecting spheres
    for (int i = 1; i < 20; ++i) {
        for (int j = 1; j < 20; ++j) {
            for (int k = 1; k < 20; ++k) {
                center = Eigen::Vector3d(i, j, k);
                auto shape = new Ellipsoid3D(center, vals, vects);
                shapes.emplace_back(shape);
                tree.addData(shape);
            }
        }
    }

    // check that the data was inserted correctly
    size_t data_size = 0;
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        data_size += it->getData().size();
    }

    BOOST_CHECK_EQUAL(data_size, 19*19*19);

    // check that they don't intersect!
    BOOST_CHECK_EQUAL(tree.getPossibleCollisions().size(), 0);

    // add some spheres which will intersect
    center = Eigen::Vector3d(1.5, 1.5, 1.5);
    vals = Eigen::Vector3d(0.2, 0.2, 0.2);
    auto shape = new Ellipsoid3D(center, vals, vects);
    shapes.emplace_back(shape);
    tree.addData(shape);

    BOOST_CHECK_EQUAL(tree.getPossibleCollisions().size(), 6);

    shape = nullptr;

    // clear the list
    for(auto shape: shapes) {
        delete shape;
    }
}

BOOST_AUTO_TEST_CASE(Test_NDTree)
{
    uint maxStorage(10);

    // Build up a NDTree with (0,0,0) as lower corner and (100,100,100) as upper corner
    NDTree<double,3> tree({0,0,0},{100,100,100});
    tree.setMaxStorage(maxStorage);

    std::uniform_real_distribution<> d1(0,50), d2(50,100);
    std::mt19937 gen;

    std::vector<AABB<double,3>> bb;
    bb.reserve(100);

    for (uint i=0;i<=maxStorage;++i) {
        Vector3d v1(d1(gen),d1(gen),d1(gen));
        Vector3d v2(d2(gen),d2(gen),d2(gen));
        bb.push_back(AABB<double,3>(v1,v2));
        // Test: the root node has no children until it is not splitted
        BOOST_CHECK_EQUAL(tree.hasChildren(),false);
        tree.addData(&bb[i]);
        if (i < maxStorage) {
            // Test: the root node has some data until it is not splitted
            BOOST_CHECK_EQUAL(tree.hasData(),true);
        }
        else {
            BOOST_CHECK_EQUAL(tree.hasChildren(), true);
        }
    }

    // Test: the root node has some children once it has been splitted
    BOOST_CHECK_EQUAL(tree.hasChildren(),true);

    // Test: the root node does not have any data anymore once it has been splitted
    BOOST_CHECK_EQUAL(tree.hasData(),false);

//    //Test: the iterator on a NDTree begins with the root node
//    NDTree<double,3>::iterator it(tree.begin());

//    Eigen::Vector3d center(it->getAABBCenter());
//    BOOST_CHECK_CLOSE(center(0),50.0,tolerance);
//    BOOST_CHECK_CLOSE(center(1),50.0,tolerance);
//    BOOST_CHECK_CLOSE(center(2),50.0,tolerance);

//    Eigen::Vector3d lower((*it).getLower());
//    BOOST_CHECK_CLOSE(lower(0),0.0,tolerance);
//    BOOST_CHECK_CLOSE(lower(1),0.0,tolerance);
//    BOOST_CHECK_CLOSE(lower(2),0.0,tolerance);

//    Eigen::Vector3d upper((*it).getUpper());
//    BOOST_CHECK_CLOSE(upper(0),100.0,tolerance);
//    BOOST_CHECK_CLOSE(upper(1),100.0,tolerance);
//    BOOST_CHECK_CLOSE(upper(2),100.0,tolerance);

//    Eigen::Vector3d extents((*it).getAABBExtents());
//    BOOST_CHECK_CLOSE(extents(0),100.0,tolerance);
//    BOOST_CHECK_CLOSE(extents(1),100.0,tolerance);
//    BOOST_CHECK_CLOSE(extents(2),100.0,tolerance);

    std::vector<AABB<double,3>>::const_iterator it1;

    // Remove all the data stored in the NDTree
    for (it1=bb.begin();it1!=bb.end();++it1) {
        tree.removeData(&(*it1));
    }
    // Test: the root node amd its children does not have any data anymore once all the data have been removed
    BOOST_CHECK_EQUAL(tree.hasData(),false);

    collision_test();
}
