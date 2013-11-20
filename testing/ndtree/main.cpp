#include <iostream>
#include <random>

#include <boost/numeric/ublas/assignment.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/random/uniform_real.hpp>

#include "aabb.h"
#include "hcoords.h"
#include "ndtree.h"
#include "hmatrix.h"

using namespace SX::Geometry;
using namespace boost::numeric::ublas;

int main()
{
    typedef AABB<double,3> BB3D;

	bounded_vector<double,3> v1,v2;
	v1<<=0,0,0;
	v2<<=1000,1000,1000;

	NDTree<double,3> tree(v1,v2);
	std::vector<BB3D> vb;
	vb.reserve(10000000);

	std::uniform_real_distribution<> d(5,995);
	std::mt19937 gen;

    for (int i=0;i<100000;++i)
    {
    	v1<<=d(gen),d(gen),d(gen);
        v2<<=1,1,1;
        v2 += v1;
        BB3D* ptr = new BB3D(v1,v2); // OK but (very ?) slow
        tree.addData(ptr);
    }
    std::cout<<tree._nSplits<<std::endl;
    std::cout<<tree<<std::endl;

//    bounded_vector<double,5> v1;
//    v1<<=1,2,3,4,5;
//    HCoords<double,5> coord(v1);
//    std::cout<<coord<<std::endl;
//
//    HCoords<double,5> coord1(coord);
//    std::cout<<coord1<<std::endl;
//
//    HCoords<double,5> coord2(20);
//    std::cout<<coord2<<std::endl;
//
//    coord2 = v1;
//    std::cout<<coord2<<std::endl;
//
//    HCoords<double,5> coord3(HCoords<double,5>(40));
//    std::cout<<coord3<<std::endl;
//    coord3 = HCoords<double,5>(50);
//	std::cout<<coord3<<std::endl;
//
//	HMatrix<double,4> mat;
//	bounded_vector<double,3> tv;
//	tv<<=2,2,2;
//	std::cout<<mat<<std::endl;
//	mat.scaling(tv);
//	std::cout<<mat<<std::endl;
//	mat.translation(tv);
//	std::cout<<mat<<std::endl;

}
