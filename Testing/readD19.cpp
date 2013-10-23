#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/config.hpp>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <boost/python/object.hpp>
#include <boost/spirit/include/qi.hpp>

#include <numpy/arrayobject.h>

#include "Blob2D.h"
#include "Blob2DFinder.h"
#include "Blob3DFinder.h"
#include "Cluster.h"
#include "Ellipse.h"
#include "Matrix33.h"
#include "MetaData.h"
#include "MetaDataWrapper.h"
#include "MMILLAsciiReader.h"
#include "RotAxis.h"
#include "Units.h"
#include "V3D.h"

using namespace boost;
using namespace SX::Geometry;
using namespace SX::Units;

// Attempt to read D19 ascii files using standard C++ is relatively slow (at
// least with iostream operations). It seems that boost spirit is extremely fast
// to parse the section of file that contains data, even faster than atoi().
// This read a 500 Mb file in about 1.0 sec. Memory mapped files seems to increae
// speed reading by a factor of 2.

typedef std::vector<int> vint;


inline PyObject* vectorToNumpyMatrix(const vint& v,int ncols, int nrows)
{
	npy_intp dim[2]={ncols,nrows};
	PyArrayObject * ndmatrix = (PyArrayObject*)PyArray_SimpleNewFromData(2, dim, NPY_INT,(void*)&(v[0]));
	ndmatrix->flags &= ~NPY_WRITEABLE;
	return (PyObject*)ndmatrix;
}

inline PyObject* V3DToNumpy(const std::vector<V3D>& v)
{
    npy_intp dim[2]={v.size(),3};
	PyObject* ndarray=PyArray_ZEROS(2,dim,NPY_DOUBLE,0);
	for (size_t i=0;i<v.size();i++)
	{
		for (size_t j=0;j<3;j++)
		{
		// Access element number i, and cast from void* to double*
		double* temp=(double*)PyArray_GETPTR2(ndarray,i,j);
		// Copy element of v into the numpy vector
		*temp=(v[i])[j];
		}
	}
	return ndarray;
}

inline PyObject* BlobMapToNumPy(const blob2DCollection& m)
{
	npy_intp dim[2]={m.size(),6};
	//Create an empty vector of the same size
	PyObject* ndarray=PyArray_ZEROS(2,dim,NPY_DOUBLE,0);
	int i=0;
	for (auto it=m.begin();it!=m.end();++it)
	{
		const Blob2D& p=it->second;
		double xc, yc, sa, sb, angle;
		p.toEllipse(xc,yc,sa,sb,angle);
		angle*=180.0/M_PI;
		// Access element number i, and cast from void* to double*
		double* temp=(double*)PyArray_GETPTR2(ndarray,i,0);
		// Copy element of v into the numpy vector
		*temp=xc;
		temp=(double*)PyArray_GETPTR2(ndarray,i,1);
		// Copy element of v into the numpy vector
		*temp=yc;
		temp=(double*)PyArray_GETPTR2(ndarray,i,2);
		// Copy element of v into the numpy vector
		*temp=sa;
		temp=(double*)PyArray_GETPTR2(ndarray,i,3);
		// Copy element of v into the numpy vector
		*temp=sb;
		temp=(double*)PyArray_GETPTR2(ndarray,i,4);
		// Copy element of v into the numpy vector
		*temp=angle;
		temp=(double*)PyArray_GETPTR2(ndarray,i++,5);
		*temp=p.getMass();
	}
	return ndarray;
}


class Scan2D
{
    public:
    Scan2D():_nframes(0),_frames(),_sum()
    {
    }
    void readFromFile(const std::string& filename)
    {
        SX::MMILLAsciiReader mm(filename.c_str());
	    _meta=mm.readMetaDataBlock();

	    _wave=_meta->getKey<double>("wavelength");
	    _gamma=_meta->getKey<double>("2theta(gamma)");
	    _scanstart=_meta->getKey<double>("scanstart");
	    _scanstep=_meta->getKey<double>("scanstep");
	    std::string instrument=_meta->getKey<std::string>("Instrument");
	    if (instrument.compare("D19")==0)
	    {
	    	_nrows=256;
	    	_ncols=640;
	    }
	    else if (instrument.compare("D10")==0 || instrument.compare("D9")==0)
	    {
	    	_nrows=32;
	    	_ncols=32;
	    }
	    std::cout << "Instrument:" << instrument << std::endl;
	    std::cout << "Wavelength:" << _wave << std::endl;
	    std::cout << "Gamma:" << _gamma << std::endl;
	    std::cout << "Start Omega:" << _scanstart << std::endl;
	    std::cout << "Step omega:" << _scanstep << std::endl;
	    std::cout << "Number of frames:" << _meta->getKey<int>("npdone") << std::endl;

	    _frames.resize(mm.nBlocks());
	    _sum.resize(mm.nBlocks());
	    _nframes=mm.nBlocks();
	    #pragma omp parallel for
	    for (std::size_t i=0;i<mm.nBlocks();++i)
	    {
		    _frames[i]=std::move(mm.readBlock(i));
		    _sum[i]=std::accumulate(_frames[i].begin(),_frames[i].end(),0);
	    }

	}


    PyObject* labelling(int frame, double s2n)
	{
	    int max=s2n*_sum[frame]/(_nrows*_ncols);
	    vint& m=_frames[frame];
	    int* ptr=&m[0];
	    blob2DCollection blobs=findBlobs2D<int>(ptr,_nrows,_ncols,max,10,1000,0);
	    //for (auto it=blobs.begin();it!=blobs.end();)
	    //{
	    //    Blob2D& p=it->second;
	    //    if (p.getMaximumMass()<6.0*max)
	    //        it=blobs.erase(it);
	    //    else
	    //        it++;
	    //}
	    std::cout << "Found " << blobs.size() << " blobs \n";

	    return BlobMapToNumPy(blobs);
	}

	void setWavelength(double lam)
	{
	    _wave=lam;
	}

	int labelling3D(double s2n, int minComp=20, int maxComp=1000, double confidence=0.9)
	{
	    std::vector<int*> ptr;
	    for (int i=0;i<_nframes;++i)
	    {
	        vint& m=_frames[i];
	        ptr.push_back(&m[0]);
	    }
	    int total=std::accumulate(_sum.begin(),_sum.end(),0);
	    double z=static_cast<double>(total)/(_nrows*_ncols*_nframes);
	    std::cout << "Search peaks above" << z*s2n << std::endl;
	    blob3DCollection blobs=findBlobs3D<int>(ptr,_nrows,_ncols,s2n*z,minComp,maxComp,0);
	    std::cout << "Found " << blobs.size() << " peaks" <<  std::endl;
	    for (auto it=blobs.begin();it!=blobs.end();++it)
	    {
	        std::cout << (it->second);
	    }

	    // Convert to Q
//	    double R=0.764;
//        double h=0.40;

//	    UnitCellFinder finder(0.1,0.01);
//	    std::vector<V3D> points;
//	    for (auto it=blobs.begin();it!=blobs.end();++it)
//	    {
//
//	        V3D v=it->second.getCenterOfMass();
//	        double gamma=(((640.0-v[0])/640.0)*120+(_gamma-60.))*deg;
//            double nu=atan2((0.5-v[1]/256.0)*h,R);
//            double Qx=cos(nu)*sin(gamma);
//            double Qy=cos(nu)*cos(gamma)-1.0;
//            double Qz=sin(nu);
//            double omega=(_scanstart+v[2]*_scanstep)*deg;
//            double newQx=Qx*cos(omega)-Qy*sin(omega);
//            double newQy=Qx*sin(omega)+Qy*cos(omega);
//            double newQz=Qz;
//            newQx/=_wave;
//            newQy/=_wave;
//            newQz/=_wave;
//	        finder.addPeak(V3D(newQx,newQy,newQz));
//	        points.push_back(V3D(newQx,newQy,newQz));
//	    }
//	    finder.run(1.0);
//	    std::cout << "Found " << finder.getNumberOfClusters() << " clusters.";


//	    const std::multimap<double,Cluster>& mm=finder.getClusters();
//	    int total=0;

//        finder.determineLattice(20);
//	    return V3DToNumpy(points);

		for (int i=0; i<_nframes; ++i)
		{
			_ellipses.insert(std::pair<int,ellipseVector>(i,ellipseVector()));
			ellipseVector& e = _ellipses[i];
			for (auto b_it=blobs.begin(); b_it!=blobs.end(); ++b_it)
			{
				V3D center, semi_axes, axis1, axis2;
				bool test = b_it->second.intersectionWithPlane(0,0,1,i,center,semi_axes,axis1,axis2, confidence);
				if (test)
				{
					Ellipse ell(center,semi_axes, axis1, axis2);
					e.push_back(ell);
				}
			}


		}

		return blobs.size();

	}

    PyObject* getEllipses(int idx)
    {

    	std::cout<<"Frame:" <<idx<<"  "<<_ellipses[idx].size()<<std::endl;
    	auto e_it = _ellipses.find(idx);
    	if (e_it != _ellipses.end())
    	{
    		int nEllipses = e_it->second.size();
			std::cout<<nEllipses<<std::endl;
        	npy_intp dim[2]={nEllipses,6};
        	PyObject* ndarray=PyArray_ZEROS(2,dim,NPY_DOUBLE,0);
			for (size_t i=0; i < nEllipses;i++)
			{
				Ellipse& ell = e_it->second[i];
				const V3D& center = ell.getCenter();
				const V3D& axes = 2.0*ell.getSemiAxes();
				const V3D& axis1 = ell.getAxis1();
				double angle = atan2(axis1[1],axis1[0])*180.0/M_PI;

				double* temp=(double*)PyArray_GETPTR2(ndarray,i,0);
				*temp = center[0];

				temp=(double*)PyArray_GETPTR2(ndarray,i,1);
				*temp = center[1];

				temp=(double*)PyArray_GETPTR2(ndarray,i,2);
				*temp = center[2];

				temp=(double*)PyArray_GETPTR2(ndarray,i,3);
				*temp = axes[0];

				temp=(double*)PyArray_GETPTR2(ndarray,i,4);
				*temp = axes[1];

				temp=(double*)PyArray_GETPTR2(ndarray,i,5);
				*temp = angle;
			}

			return ndarray;
    	}

    return 0;

    }


    PyObject* getFrame(int i)
    {
        vectorToNumpyMatrix(_frames[i],_ncols,_nrows);
    }

    std::size_t getCounts(int i) const
    {
        return _sum[i];
    }
    PyObject* getKey(const std::string& name)
    {
        return getPythonKey(_meta,name);
    }

    // Return a selected meta data key as a python string
    PyObject* getKeyAsString(const std::string& name)
    {
        return getPythonKeyAsString(_meta,name);
    }

    int getNFrames()
    {
    	return _nframes;
    }

    // Return all the meta data keys available as a python list
    boost::python::list getAllKeys()
    {

    	boost::python::list l;

    	const std::set<std::string>& keys = _meta->getAllKeys();

    	std::set<std::string>::const_iterator it;

    	for (it = keys.begin(); it != keys.end(); ++it)
    	{
    		l.append(*it);
    	}
		return l;

    }


    private:
    int _nframes;
    double _wave;
    double _gamma;
    double _scanstart;
    double _scanstep;
    std::vector<vint> _frames;
    std::vector<int> _sum;
	SX::MetaData* _meta;
	typedef std::vector<Ellipse> ellipseVector;
	std::map<int,ellipseVector> _ellipses;
	int _nrows, _ncols;
};



BOOST_PYTHON_MODULE(libD19)
{
    using namespace boost::python;

    import_array();
    boost::python::numeric::array::set_module_and_type("numpy","ndarray");

    class_<Scan2D>("Scan2D",init<>())
    .def("readFromFile",&Scan2D::readFromFile)
	.def("getFrame", &Scan2D::getFrame)
	.def("labelling",&Scan2D::labelling)
	.def("labelling3D",&Scan2D::labelling3D)
	.def("setWavelength",&Scan2D::setWavelength)
	.def("getCounts",&Scan2D::getCounts)
	.def("getKey",&Scan2D::getKey)
	.def("getKeyAsString",&Scan2D::getKeyAsString)
	.def("getAllKeys",&Scan2D::getAllKeys)
	.def("getNFrames",&Scan2D::getNFrames)
	.def("getEllipses",&Scan2D::getEllipses)
	;
}
