#include <cmath>
#include <vector>
#include <iostream>

#include "Matrix33.h"
#include "V3D.h"
#include "Blob3D.h"
#include "Blob3DFinder.h"

using namespace SX::Geometry;

int main()
{

	typedef std::vector<int> vint;

	std::vector<vint> data;
	const int N=100;
	data.resize(N);

	for (auto it=data.begin(); it != data.end(); ++it)
	{
		it->resize(N*N);

	}

	for (int i=0; i<N; ++i)
	{
		vint& frame=data[i];
		double X = (i-50.0);
		double X2 = X*X/10.0;
		for (int j=0; j<N; ++j)
		{
			double Y = (j-50.0);
			double Y2 = Y*Y/10.0;
			for (int k=0; k<N; ++k)
			{
				double Z = (k-50.0);
				double Z2 = Z*Z/10.0;
				frame[j*N+k] = 1000.0*std::exp(-0.5*(X2 + Y2 + Z2));
			}
		}
	}
    std::vector<int*> ptr;
    for (int i=0;i<N;++i)
    {
        vint& m=data[i];
        ptr.push_back(&m[0]);
    }

    blob3DCollection blobs=findBlobs3D<int>(ptr,20,20,100,0,100000000,0);

    std::cout<<blobs.size()<<std::endl;

    V3D center;
    V3D semi_axes;
    V3D axis1, axis2;

    blobs[1].intersectionWithPlane(0.0,0.0,1.0,10.0, center, semi_axes, axis1, axis2);

    std::cout<<"INTERSECTION ELLIPSE CENTER "<<center<<std::endl;
    std::cout<<"INTERSECTION ELLIPSE SEMI AXES "<<semi_axes<<std::endl;
    std::cout<<"INTERSECTION ELLIPSE AXIS 1 "<<axis1<<std::endl;
    std::cout<<"INTERSECTION ELLIPSE AXIS 2 "<<axis2<<std::endl;

	return 0;
}
