#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <boost/spirit/include/qi.hpp>
#include <boost/python.hpp>
#include <numpy/arrayobject.h>
#include <boost/python/object.hpp>
#include <boost/python/numeric.hpp>
#include <string>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/numeric/mtl/mtl.hpp>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <boost/config.hpp>
#include <utility>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <unordered_map>
#include <queue>
#include "Blob2D.h"
#include "Blob2DFinder.h"
#include "Blob3DFinder.h"


using namespace mtl;
using namespace boost::interprocess;
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
using namespace boost;
using namespace SX::Geometry;

struct Timer
{
	Timer():t1(0),tot(0){}
	void start()
	{
		t1=clock();
	}
	void stop()
	{
		tot+=clock()-t1;
	}
	friend std::ostream& operator<<(std::ostream& os, Timer& t)
	{
		os << static_cast<double>(t.tot)/CLOCKS_PER_SEC;
		return os;
	}
	clock_t t1, tot;
};

int f2(int a)
{
    int b;
    for (int i=0;i<1000000;++i)
        b+=a;
    return b;
}
// Attempt to read D19 ascii files using standard C++ is relatively slow (at
// least with iostream operations). It seems that boost spirit is extremely fast
// to parse the section of file that contains data, even faster than atoi().
// This read a 500 Mb file in about 1.0 sec. Memory mapped files seems to increae
// speed reading by a factor of 2.

typedef dense2D<double,matrix::parameters<tag::col_major> > mat;
typedef dense2D<int,matrix::parameters<tag::col_major> > matint;
typedef dense2D<bool,matrix::parameters<tag::col_major> > matbool;
typedef adjacency_list <vecS, vecS, undirectedS> Graph;
typedef std::vector<int> vint;



void readDoublesFromChar(const char*, const char*, std::vector<double>&);

inline PyObject* ublasMatrixToNumpy(const matint& m)
{
	npy_intp dim[2]={num_rows(m),num_cols(m)};
	PyArrayObject * ndmatrix = (PyArrayObject*)PyArray_SimpleNewFromData(2, dim, NPY_INT,(void*)&(m(0,0)));
	ndmatrix->flags &= ~NPY_WRITEABLE;
	return (PyObject*)ndmatrix;
}

inline PyObject* vectorToNumpyMatrix(const vint& v,int ncols, int nrows)
{
	npy_intp dim[2]={ncols,nrows};
	PyArrayObject * ndmatrix = (PyArrayObject*)PyArray_SimpleNewFromData(2, dim, NPY_INT,(void*)&(v[0]));
	ndmatrix->flags &= ~NPY_WRITEABLE;
	return (PyObject*)ndmatrix;
}

inline PyObject* copyublasMatrixToNumpy(const mat& m)
{
	npy_intp dim[2]={num_cols(m),num_rows(m)};
	//std::cout << num_cols(m) << num_rows(m);
	//Create an empty vector of the same size
	PyObject* ndarray=PyArray_ZEROS(2,dim,NPY_DOUBLE,0);
	for (size_t i=0;i<num_cols(m);i++)
	{
		for (size_t j=0;j<num_rows(m);j++)
		{
		// Access element number i, and cast from void* to double*
		double* temp=(double*)PyArray_GETPTR2(ndarray,i,j);
		// Copy element of v into the numpy vector
		*temp=m(j,i);
		}
	}
	return ndarray;
}

inline PyObject* BlobMapToNumPy(const blob2DCollection& m)
{
	npy_intp dim[2]={m.size(),5};
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
		temp=(double*)PyArray_GETPTR2(ndarray,i++,4);
		// Copy element of v into the numpy vector
		*temp=angle;
	}
	return ndarray;
}

inline PyObject* copyublasMatrixToNumpyInt(const matint& m)
{
	npy_intp dim[2]={num_cols(m),num_rows(m)};
	//std::cout << num_cols(m) << num_rows(m);
	//Create an empty vector of the same size
	PyObject* ndarray=PyArray_ZEROS(2,dim,NPY_INT,0);
	for (size_t i=0;i<num_cols(m);i++)
	{
		for (size_t j=0;j<num_rows(m);j++)
		{
		// Access element number i, and cast from void* to double*
		double* temp=(double*)PyArray_GETPTR2(ndarray,i,j);
		// Copy element of v into the numpy vector
		*temp=m(j,i);
		}
	}
	return ndarray;
}


inline void readIntsFromChar(const char* b, const char* e, std::vector<int>& v)
{
   qi::phrase_parse(b, e,
        *qi::int_ >> qi::eoi, ascii::space, v);
}
inline void readDoublesFromChar(const char* b, const char* e, std::vector<double>& v)
{
   qi::phrase_parse(b, e,
        *qi::double_ >> qi::eoi, ascii::space, v);
}

inline int readIntsUsingAtoi(const char* b, mat& m)
{
    char number[8];
    int offset=0;
    int count=0;
    int sum=0;
    for (int i=0;i<640;++i)
    {
        for (int j=0;j<256;++j)
        {
            memcpy(number,b+offset,8);
            m(i,j)=atoi(number);
            sum+=m(i,j);
            if (++count==10)
            {
                offset+=1;
                count=0;
             }
            offset+=8;
        }

    }
    return sum;
}




class Scan2D
{
    public:
    Scan2D(int nframes):_nframes(nframes),_frames(nframes),_sum(nframes)
    {
       for (int i=0;i<nframes;++i)
        _frames[i].reserve(256*640);
    }
    void readFromFile(const std::string& filename)
    {
            try
            {
            // Do a mapping of the file
			file_mapping m_file(filename.c_str(),read_only);

	        std::size_t header_offset;
	        if (_nframes!=1)
			    header_offset=42*81;
			else
			    header_offset=43*81;
			std::size_t block_size=16384*81;
			std::size_t frame_size=16392*81;
			#pragma omp parallel for
			for (int i=0;i<_nframes;++i)
			{

			    // Map a region corresponding to a frame
			    mapped_region region1(m_file,read_only,header_offset+frame_size*i,frame_size);
			    const char* b=reinterpret_cast<char*>(region1.get_address());
			    // Get the virtual memory address of this block
                const char* b1=b+8*81;
                readIntsFromChar(b1,b1+block_size,_frames[i]);
                _sum[i]=std::accumulate(_frames[i].begin(),_frames[i].end(),0);
			}
			}catch(...)
			{
			    throw "Problem with memory mapped file";
			}
	}


	PyObject* labelling(int frame, double s2n)
	{
	    int max=s2n*_sum[frame]/(640.0*256.0);
	    vint& m=_frames[frame];
	    int* ptr=&m[0];
	    blob2DCollection blobs=findBlobs2D<int>(ptr,256,640,max,10,1000,0);
	    for (auto it=blobs.begin();it!=blobs.end();)
	    {
	        Blob2D& p=it->second;
	        if (p.getMaximumMass()<6.0*max)
	            it=blobs.erase(it);
	        else
	            it++;
	    }
	    std::cout << "Found " << blobs.size() << " blobs \n";
	    for (auto it=blobs.begin();it!=blobs.end();++it)
	    {
	        Blob2D& p=it->second;
	        std::cout << p.getMaximumMass() << std::endl;
	    }
	    return BlobMapToNumPy(blobs);
	}
	PyObject* labelling3D(double s2n)
	{
	    std::vector<int*> ptr;
	    for (int i=0;i<_nframes;++i)
	    {
	        vint& m=_frames[i];
	        ptr.push_back(&m[0]);
	    }
	    blob3DCollection blobs=findBlobs3D<int>(ptr,256,640,s2n,20,1000,0);
	    std::cout << "Found" << blobs.size() << " peaks" <<  std::endl;
	    // Results
	    mat result;
	    result.change_dim(16,blobs.size());
	    int i=0;
	    for (auto it=blobs.begin();it!=blobs.end();++it)
	    {
	        V3D com;
	        V3D axes;
	        V3D v1, v2, v3;
	        it->second.toEllipsoid(com,axes,v1,v2,v3);
	        double m=it->second.getMass();
	        result(0,i)=com[0];
	        result(1,i)=com[1];
	        result(2,i)=com[2];
	        result(3,i)=m;
	        result(4,i)=axes[0];
	        result(5,i)=axes[1];
	        result(6,i)=axes[2];
	        result(7,i)=v1[0];
	        result(8,i)=v1[1];
	        result(9,i)=v1[2];
	        result(10,i)=v2[0];
	        result(11,i)=v2[1];
	        result(12,i)=v2[2];
	        result(13,i)=v3[0];
	        result(14,i)=v3[1];
	        result(15,i++)=v3[2];
	    }
	    return copyublasMatrixToNumpy(result);
	}

    PyObject* getFrame(int i)
    {
        vectorToNumpyMatrix(_frames[i],640,256);
    }

    private:
    int _nframes;
    std::vector<vint> _frames;
    std::vector<int> _sum;

};



BOOST_PYTHON_MODULE(libD19)
{
    using namespace boost::python;

    import_array();
    boost::python::numeric::array::set_module_and_type("numpy","ndarray");

    class_<Scan2D>("Scan2D",init<int>())
    .def("readFromFile",&Scan2D::readFromFile)
	.def("getFrame", &Scan2D::getFrame)
	.def("labelling",&Scan2D::labelling)
	.def("labelling3D",&Scan2D::labelling3D)
	;
}

