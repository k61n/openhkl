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
#include <boost/math/special_functions/erf.hpp>
#include <numpy/arrayobject.h>

#include "MetaData.h"
#include "MetaDataWrapper.h"
#include "ILLAsciiDataReader.h"
#include "Units.h"
#include <Eigen/Dense>

using namespace boost;
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




class Scan2D
{
    public:
    Scan2D():_nframes(0),_frames(),_sum()
    {
    }
    void readFromFile(const std::string& filename)
    {
        SX::Data::IDataReader* mm=SX::Data::ILLAsciiDataReader::create();
        mm->open(filename.c_str());
	    _meta=mm->getMetaData();

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

	    
	    _nframes=mm->nFrames();
	    _frames.resize(_nframes);
	    _sum.resize(_nframes);
	    #pragma omp parallel for
	    for (std::size_t i=0;i<mm->nFrames();++i)
	    {
		    _frames[i]=std::move(mm->getFrame(i));
		    _sum[i]=std::accumulate(_frames[i].begin(),_frames[i].end(),0);
	    }

	}



	void setWavelength(double lam)
	{
	    _wave=lam;
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
	SX::Data::MetaData* _meta;
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
	.def("setWavelength",&Scan2D::setWavelength)
	.def("getCounts",&Scan2D::getCounts)
	.def("getKey",&Scan2D::getKey)
	.def("getKeyAsString",&Scan2D::getKeyAsString)
	.def("getAllKeys",&Scan2D::getAllKeys)
	.def("getNFrames",&Scan2D::getNFrames)
	;
}
