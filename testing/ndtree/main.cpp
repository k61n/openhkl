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

using namespace nsx::Geometry;
using namespace boost::numeric::ublas;

int main()
{

	const std::size_t dim=2;
    typedef AABB<double,dim> BB2D;

    const std::size_t N=1e5;

	bounded_vector<double,dim> v1,v2;
	v1<<=0,0;
	v2<<=100,100;

	NDTree<double,dim> tree(v1,v2);
	tree.setDepth(6);
	tree.setMaxStorage(6);

	std::vector<BB2D> data;
	data.reserve(N);

	std::uniform_real_distribution<> d(5,95);
	std::mt19937 gen;

    for (int i=0;i<N;++i)
    {
    	v1<<=d(gen),d(gen);
        v2<<=1,1;
        v2 += v1;
        data.push_back(BB2D(v1,v2));
    }

    for (int i=0;i<N;++i)
    {
        tree.addData(&(data[i]));
    }

	std::vector<NDTree<double,dim>::data_range_pair> treeData;
	treeData.reserve(100000);
	tree.getData(treeData);

	int intersection=0;
	for(auto it=treeData.begin();it!=treeData.end();++it)
	{
		nsx::Geometry::NDTree<double,dim>::data_range_pair& p=*it;
		std::size_t d=std::distance(p.first,p.second);
		intersection += d*(d-1)/2;
			//std::cout<<*(*it2)<<std::endl;
	}
	std::cout << " Number of intersections" << intersection << std::endl;
	std::cout << " Number of intersections brut force" << N*(N-1)/2 << std::endl;


//    std::cout<<tree<<std::endl;

//	typedef typename std::vector<AABB<double,3>*>::iterator data_iterator;
//	typedef std::pair< data_iterator , data_iterator > data_range_pair;

//    std::vector<nsx::Geometry::NDTree<double,dim>::data_range_pair> data;

//    data = tree.getData();

//    int intersection=0;
//    for(auto it=data.begin();it!=data.end();++it)
//    {
//    	nsx::Geometry::NDTree<double,dim>::data_range_pair& p=*it;
//    	std::size_t d=std::distance(p.first,p.second);
//    	intersection+=d*(d-1)/2;
//    		//std::cout<<*(*it2)<<std::endl;
//    }
//    std::cout << " Number of intersections" << intersection << std::endl;
//    std::cout << " Number of intersections brut force" << N*(N-1)/2 << std::endl;
//
//    std::cout<<data.size()<<std::endl;


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
