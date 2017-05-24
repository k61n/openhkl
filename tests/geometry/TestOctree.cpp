#define BOOST_TEST_MODULE "Test NDTree"
#define BOOST_TEST_DYN_LINK

#include <cmath>
#include <random>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include <nsxlib/geometry/AABB.h>
#include <nsxlib/geometry/Octree.h>
#include <nsxlib/geometry/Ellipsoid.h>

const double tolerance=1e-5;

void collision_test()
{
    nsx::Octree tree({0,0,0},{100,100,100});
    auto vects = Eigen::Matrix3d::Identity();
    const double radius = 0.45;
    auto vals = Eigen::Vector3d(radius, radius, radius);
    auto center = Eigen::Vector3d(0.0, 0.0, 0.0);

    std::set<const nsx::IShape*> test_set;

    std::vector<nsx::Ellipsoid*> shapes;

    // lattice of non-intersecting spheres
    for (int i = 1; i < 20; ++i) {
        for (int j = 1; j < 20; ++j) {
            for (int k = 1; k < 20; ++k) {
                center = Eigen::Vector3d(i, j, k);
                auto shape = new nsx::Ellipsoid(center, vals, vects);
                shapes.emplace_back(shape);
                tree.addData(shape);
            }
        }
    }

    // check that the data was inserted correctly
    unsigned int numChambers = 0;
    for (auto&& chamber: tree) {
        numChambers += 1;
        for (auto&& shape: chamber.getData()) {
            test_set.insert(shape);
        }
    }

    BOOST_CHECK_EQUAL(numChambers, tree.numChambers());

    BOOST_CHECK_EQUAL(test_set.size(), 19*19*19);

    // check that they don't intersect!
    BOOST_CHECK_EQUAL(tree.getCollisions().size(), 0);

    // add some spheres which will intersect
    center = Eigen::Vector3d(1.5, 1.5, 1.5);
    vals = Eigen::Vector3d(radius, radius, radius);
    auto shape = new nsx::Ellipsoid(center, vals, vects);


    BOOST_CHECK_EQUAL(tree.getCollisions(*shape).size(), 8);
    shapes.emplace_back(shape);
    tree.addData(shape);
    BOOST_CHECK_EQUAL(tree.getCollisions().size(), 8);

    shape = nullptr;

    // clear the list
    for(auto shape: shapes) {
        delete shape;
    }
}

void split_test()
{
    nsx::Octree tree({0,0,0},{50,50,50});
    tree.setMaxStorage(4);

    auto vects = Eigen::Matrix3d::Identity();
    const double radius = 1.0;
    auto vals = Eigen::Vector3d(radius, radius, radius);

    std::set<const nsx::IShape*> test_set;

    std::vector<nsx::Ellipsoid*> shapes;

    shapes.emplace_back(new nsx::Ellipsoid(Eigen::Vector3d(12.5, 12.5, 12.5), vals, vects));
    shapes.emplace_back(new nsx::Ellipsoid(Eigen::Vector3d(12.5, 12.5, 37.5), vals, vects));
    shapes.emplace_back(new nsx::Ellipsoid(Eigen::Vector3d(12.5, 37.5, 12.5), vals, vects));
    shapes.emplace_back(new nsx::Ellipsoid(Eigen::Vector3d(12.5, 37.5, 37.5), vals, vects));
    shapes.emplace_back(new nsx::Ellipsoid(Eigen::Vector3d(37.5, 12.5, 12.5), vals, vects));
    shapes.emplace_back(new nsx::Ellipsoid(Eigen::Vector3d(37.5, 12.5, 37.5), vals, vects));
    shapes.emplace_back(new nsx::Ellipsoid(Eigen::Vector3d(37.5, 37.5, 12.5), vals, vects));
    shapes.emplace_back(new nsx::Ellipsoid(Eigen::Vector3d(37.5, 37.5, 37.5), vals, vects));

    for (auto&& shape: shapes) {
        tree.addData(shape);
    }

    // check that it split properly
    BOOST_CHECK_EQUAL(tree.numChambers(), 8);

    // check that the data was inserted correctly
    for (auto&& chamber: tree) {
        BOOST_CHECK_EQUAL(chamber.getData().size(), 1);
    }

    // check that the collisions with chambers make sense
    for (auto&& shape: shapes) {
        unsigned int num_intercept = 0;

        for (auto&& chamber: tree) {
            BOOST_CHECK_EQUAL(shape->intercept(chamber), chamber.intercept(*shape));

            if (shape->intercept(chamber)) {
                ++num_intercept;
            }
        }

        BOOST_CHECK_EQUAL(num_intercept, 1);
    }

    // clear the list
    for(auto shape: shapes) {
        delete shape;
    }
}

BOOST_AUTO_TEST_CASE(Test_NDTree)
{
    unsigned int maxStorage(10);

    // Build up a NDTree with (0,0,0) as lower corner and (100,100,100) as upper corner
    nsx::Octree tree({0,0,0},{100,100,100});
    tree.setMaxStorage(maxStorage);

    std::uniform_real_distribution<> d1(0,50), d2(50,100);
    std::mt19937 gen;

    std::vector<nsx::AABB> bb;
    bb.reserve(100);

    for (unsigned int i=0;i<=maxStorage;++i) {
        Eigen::Vector3d v1(d1(gen),d1(gen),d1(gen));
        Eigen::Vector3d v2(d2(gen),d2(gen),d2(gen));
        bb.push_back(nsx::AABB(v1,v2));
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

    std::vector<nsx::AABB>::const_iterator it1;

    // Remove all the data stored in the NDTree
    for (it1=bb.begin();it1!=bb.end();++it1) {
        tree.removeData(&(*it1));
    }
    // Test: the root node amd its children does not have any data anymore once all the data have been removed
    BOOST_CHECK_EQUAL(tree.hasData(),false);

    collision_test();

    split_test();
}
