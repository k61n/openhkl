#define BOOST_TEST_MODULE "Test NDTree"
#define BOOST_TEST_DYN_LINK

#include <cmath>
#include <random>
#include <vector>

#include <boost/test/unit_test.hpp>

#include <Eigen/Dense>

#include "AABB.h"
#include "NDTree.h"

using namespace SX::Geometry;

using Eigen::Vector3d;

const double tolerance=1e-5;

typedef unsigned int uint;

BOOST_AUTO_TEST_CASE(Test_NDTree)
{
	uint maxStorage(10);

	NDTree<double,3>::setMaxStorage(maxStorage);

	// Build up a NDTree with (0,0,0) as lower corner and (100,100,100) as upper corner
	NDTree<double,3> tree({0,0,0},{100,100,100});

	std::uniform_real_distribution<> d1(0,50), d2(50,100);
	std::mt19937 gen;

	std::vector<AABB<double,3>> bb;
	bb.reserve(100);

	for (uint i=0;i<=maxStorage;++i)
	{
		Vector3d v1(d1(gen),d1(gen),d1(gen));
		Vector3d v2(d2(gen),d2(gen),d2(gen));
		bb.push_back(AABB<double,3>(v1,v2));
		// Test: the root node has no children until it is not splitted
		BOOST_CHECK_EQUAL(tree.hasChildren(),false);
		tree.addData(&bb[i]);
		if (i < maxStorage)
			// Test: the root node has some data until it is not splitted
			BOOST_CHECK_EQUAL(tree.hasData(),true);
	}

	// Test: the root node has some children once it has been splitted
	BOOST_CHECK_EQUAL(tree.hasChildren(),true);

	// Test: the root node does not have any data anymore once it has been splitted
	BOOST_CHECK_EQUAL(tree.hasData(),false);

	std::vector<AABB<double,3>*> voxels;
	//tree.get_voxels(voxels);
	//for (auto it=voxels.begin();it!=voxels.end();++it)



	// Remove all the data stored in the NDTree
	for (uint i=0;i<=maxStorage;++i)
		tree.removeData(&bb[i]);

	// Test: the root node amd its children does not have any data anymore once all the data have been removed
	BOOST_CHECK_EQUAL(tree.hasData(),false);
//	for (uint i=0;i<8;++i)
//		BOOST_CHECK_EQUAL(tree.children[i].hasData(),false);

	// Test: the root node still has some children once of the data has been removed
	BOOST_CHECK_EQUAL(tree.hasChildren(),true);



}
