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

	data.resize(20);

	for (auto it=data.begin(); it != data.end(); ++it)
	{
		it->resize(400);

	}

	for (int i=0; i<20; ++i)
	{
		vint& frame=data[i];
		double X = (i-10.0);
		double X2 = X*X/10.0;
		for (int j=0; j<20; ++j)
		{
			double Y = (j-10.0);
			double Y2 = Y*Y/20.0;
			for (int k=0; k<20; ++k)
			{
				double Z = (k-10.0);
				double Z2 = Z*Z/2.0;
				frame[j*20+k] = 1000.0*std::exp(-(X2 + Y2 + Z2));
			}
		}
	}
    std::vector<int*> ptr;
    for (int i=0;i<20;++i)
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
