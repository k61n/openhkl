//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      test/cpp/geometry/TestOctree.cpp
//! @brief     Test ...
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "test/cpp/catch.hpp"

#include <Eigen/Dense>
#include <cmath>
#include <random>
#include <vector>

#include "base/geometry/AABB.h"
#include "base/geometry/Ellipsoid.h"
#include "core/shape/Octree.h"

void collision_test()
{
    ohkl::Octree tree({0, 0, 0}, {100, 100, 100});
    auto vects = Eigen::Matrix3d::Identity();
    const double radius = 0.45;
    auto vals = Eigen::Vector3d(radius, radius, radius);
    auto center = Eigen::Vector3d(0.0, 0.0, 0.0);

    std::set<const ohkl::Ellipsoid*> test_set;

    std::vector<ohkl::Ellipsoid*> ellipsoids;

    // lattice of non-intersecting spheres
    for (int i = 1; i < 20; ++i) {
        for (int j = 1; j < 20; ++j) {
            for (int k = 1; k < 20; ++k) {
                center = Eigen::Vector3d(i, j, k);
                auto ellipsoid = new ohkl::Ellipsoid(center, vals, vects);
                ellipsoids.emplace_back(ellipsoid);
                tree.addData(ellipsoid);
            }
        }
    }

    // check that the data was inserted correctly
    unsigned int numChambers = 0;
    for (const auto& chamber : tree) {
        numChambers += 1;
        for (const auto& ellipsoid : chamber.getData())
            test_set.insert(ellipsoid);
    }

    CHECK(numChambers == tree.numChambers());

    CHECK(test_set.size() == 19 * 19 * 19);

    // check that they don't intersect!
    CHECK(tree.getCollisions().empty());

    // add some spheres which will intersect
    center = Eigen::Vector3d(1.5, 1.5, 1.5);
    vals = Eigen::Vector3d(radius, radius, radius);
    auto ellipsoid = new ohkl::Ellipsoid(center, vals, vects);

    CHECK(tree.getCollisions(*ellipsoid).size() == 8);
    ellipsoids.emplace_back(ellipsoid);
    tree.addData(ellipsoid);
    CHECK(tree.getCollisions().size() == 8);

    ellipsoid = nullptr;

    // clear the list
    for (auto ellipsoid : ellipsoids)
        delete ellipsoid;
}

void collision_test_2()
{
    ohkl::Octree tree({0, 0, 0}, {100, 100, 100});
    auto vects = Eigen::Matrix3d::Identity();
    const double radius = 0.55;
    auto vals = Eigen::Vector3d(radius, radius, radius);
    auto center = Eigen::Vector3d(0.0, 0.0, 0.0);

    std::set<const ohkl::Ellipsoid*> test_set;

    std::vector<ohkl::Ellipsoid*> ellipsoids;

    const int N = 19;

    // lattice of non-intersecting spheres
    for (int i = 1; i < N + 1; ++i) {
        for (int j = 1; j < N + 1; ++j) {
            for (int k = 1; k < N + 1; ++k) {
                center = Eigen::Vector3d(i, j, k);
                auto ellipsoid = new ohkl::Ellipsoid(center, vals, vects);
                ellipsoids.emplace_back(ellipsoid);
                tree.addData(ellipsoid);
            }
        }
    }

    // check that the data was inserted correctly
    unsigned int numChambers = 0;
    for (const auto& chamber : tree) {
        numChambers += 1;
        for (const auto& ellipsoid : chamber.getData())
            test_set.insert(ellipsoid);
    }

    CHECK(numChambers == tree.numChambers());
    CHECK(test_set.size() == N * N * N);

    // check that they don't intersect!
    CHECK(tree.getCollisions().size() == 3 * N * N * (N - 1));

    // add some spheres which will intersect
    center = Eigen::Vector3d(1.5, 1.5, 1.5);
    vals = Eigen::Vector3d(radius, radius, radius);
    auto ellipsoid = new ohkl::Ellipsoid(center, vals, vects);

    CHECK(tree.getCollisions(*ellipsoid).size() == 8);
    ellipsoids.emplace_back(ellipsoid);
    tree.addData(ellipsoid);
    CHECK(tree.getCollisions().size() == 3 * N * N * (N - 1) + 8);

    ellipsoid = nullptr;

    // clear the list
    for (auto ellipsoid : ellipsoids)
        delete ellipsoid;
}

void split_test()
{
    ohkl::Octree tree({0, 0, 0}, {50, 50, 50});
    tree.setMaxStorage(4);

    auto vects = Eigen::Matrix3d::Identity();
    const double radius = 1.0;
    auto vals = Eigen::Vector3d(radius, radius, radius);

    std::set<const ohkl::Ellipsoid*> test_set;

    std::vector<ohkl::Ellipsoid*> ellipsoids;

    ellipsoids.emplace_back(new ohkl::Ellipsoid(Eigen::Vector3d(12.5, 12.5, 12.5), vals, vects));
    ellipsoids.emplace_back(new ohkl::Ellipsoid(Eigen::Vector3d(12.5, 12.5, 37.5), vals, vects));
    ellipsoids.emplace_back(new ohkl::Ellipsoid(Eigen::Vector3d(12.5, 37.5, 12.5), vals, vects));
    ellipsoids.emplace_back(new ohkl::Ellipsoid(Eigen::Vector3d(12.5, 37.5, 37.5), vals, vects));
    ellipsoids.emplace_back(new ohkl::Ellipsoid(Eigen::Vector3d(37.5, 12.5, 12.5), vals, vects));
    ellipsoids.emplace_back(new ohkl::Ellipsoid(Eigen::Vector3d(37.5, 12.5, 37.5), vals, vects));
    ellipsoids.emplace_back(new ohkl::Ellipsoid(Eigen::Vector3d(37.5, 37.5, 12.5), vals, vects));
    ellipsoids.emplace_back(new ohkl::Ellipsoid(Eigen::Vector3d(37.5, 37.5, 37.5), vals, vects));

    for (const auto& ellipsoid : ellipsoids)
        tree.addData(ellipsoid);

    // check that it split properly
    CHECK(tree.numChambers() == 8);

    // check that the data was inserted correctly
    for (const auto& chamber : tree)
        CHECK(chamber.getData().size() == 1);

    // check that the collisions with chambers make sense
    for (const auto& ellipsoid : ellipsoids) {
        unsigned int num_intercept = 0;

        for (const auto& chamber : tree) {
            CHECK(ellipsoid->collide(chamber) == chamber.collide(*ellipsoid));

            if (ellipsoid->collide(chamber)) {
                ++num_intercept;
            }
        }

        CHECK(num_intercept == 1);
    }

    // clear the list
    for (auto ellipsoid : ellipsoids)
        delete ellipsoid;
}

TEST_CASE("test/geometry/TestOctree.cpp", "")
{
    unsigned int maxStorage(10);

    // Build up a NDTree with (0,0,0) as lower corner and (100,100,100) as upper
    // corner
    ohkl::Octree tree({0, 0, 0}, {100, 100, 100});
    tree.setMaxStorage(maxStorage);

    std::uniform_real_distribution<> d1(0, 50), d2(50, 100);
    std::mt19937 gen;

    std::vector<ohkl::Ellipsoid> ellipsoids;
    ellipsoids.reserve(100);

    for (unsigned int i = 0; i <= maxStorage; ++i) {
        Eigen::Vector3d v1(d1(gen), d1(gen), d1(gen));
        Eigen::Matrix3d mat;
        mat << 1, 0, 0, 0, 1, 0, 0, 0, 1;
        ellipsoids.emplace_back(ohkl::Ellipsoid(v1, mat));
        // Test: the root node has no children until it is not splitted
        CHECK(!tree.hasChildren());
        tree.addData(&ellipsoids[i]);
        if (i < maxStorage) {
            // Test: the root node has some data until it is not splitted
            CHECK(tree.hasData());
        } else {
            CHECK(tree.hasChildren());
        }
    }

    // Test: the root node has some children once it has been splitted
    CHECK(tree.hasChildren());

    // Test: the root node does not have any data anymore once it has been
    // splitted
    CHECK(!tree.hasData());

    std::vector<ohkl::Ellipsoid>::const_iterator it1;

    // Remove all the data stored in the NDTree
    for (it1 = ellipsoids.begin(); it1 != ellipsoids.end(); ++it1)
        tree.removeData(&(*it1));
    // Test: the root node amd its children does not have any data anymore once
    // all the data have been removed
    CHECK(!tree.hasData());

    collision_test();

    collision_test_2();

    split_test();
}
