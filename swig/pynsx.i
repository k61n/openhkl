%module "pynsx"
 
%include "std_shared_ptr.i"
%include "std_string.i"
%include "std_vector.i"

%template(vector_1d)  std::vector<double>;
%template(vector_2d)  std::vector<std::vector<double>>;
%template(vector_1i) std::vector<int>;
%template(vector_2i) std::vector<std::vector<int>>;

%shared_ptr(SX::Crystal::Peak3D)
%shared_ptr(SX::Chemistry::Material)
%shared_ptr(SX::Instrument::Diffractometer)


%{
#define SWIG_FILE_WITH_INIT

#include <Python.h>
#include <numpy/arrayobject.h>

#include <boost/property_tree/ptree.hpp>
 
#include <Eigen/Dense>
#include <Eigen/Geometry>

using Eigen::Quaterniond;
using Eigen::RowVector3d;
using Eigen::Vector3d;
using Eigen::Matrix3d;

namespace proptree=boost::property_tree;
namespace property_tree=boost::property_tree;

#include "utils/Enums.h"
using SX::Instrument::DataOrder;

#include "instrument/Axis.h"
#include "instrument/RotAxis.h"
#include "instrument/TransAxis.h"
#include "instrument/Gonio.h"
  
#include "instrument/ComponentState.h"
#include "instrument/Component.h"
#include "instrument/Monochromator.h"

#include "instrument/Detector.h"
#include "instrument/MonoDetector.h"
#include "instrument/CylindricalDetector.h"
#include "instrument/FlatDetector.h"
 
#include "instrument/Source.h"

#include "chemistry/Material.h"

#include "instrument/Sample.h"
using SX::Crystal::CellList;

#include "geometry/Basis.h"
#include "geometry/IShape.h"
#include "geometry/AABB.h"
#include "geometry/Ellipsoid.h"
#include "geometry/OBB.h"
#include "geometry/Sphere.h"

#include "geometry/Blob3D.h"
#include "crystal/UnitCell.h"

#include "crystal/FFTIndexing.h"
#include "crystal/GruberReduction.h"

#include "instrument/Diffractometer.h"
#include "instrument/DiffractometerStore.h"

#include "kernel/Singleton.h"

#include "data/MetaData.h"
#include "data/IDataReader.h"
#include "data/ILLDataReader.h"
%}

%include "numpy.i"
%include "eigen.i"

%init %{
    import_array();
%}


%include <boost/property_tree/ptree.hpp>



%include "instrument/Axis.h"
%include "instrument/RotAxis.h"
%include "instrument/TransAxis.h"
%include "instrument/Gonio.h"

%include "instrument/ComponentState.h"
%include "instrument/Component.h"
%include "instrument/Monochromator.h"
%include "instrument/Source.h"


%include "geometry/IShape.h"
%template(IShape3D) SX::Geometry::IShape<double,3>;

%include "geometry/AABB.h"
%template(AABB3D) SX::Geometry::AABB<double,3>;

%include "geometry/Blob3D.h"

%include "chemistry/Material.h"
%include "crystal/UnitCell.h"

%include "instrument/Detector.h"
%include "instrument/MonoDetector.h"
%include "instrument/CylindricalDetector.h"
%include "instrument/FlatDetector.h"

%include "instrument/Sample.h"
 
%include "instrument/Diffractometer.h"
 
%include "kernel/Singleton.h"

namespace SX {
   namespace Instrument {class DiffractometerStore;}
   namespace Crystal {struct tVector;}
   %template(DiffractometerStoreBase) Kernel::Singleton<Instrument::DiffractometerStore, Kernel::Constructor, Kernel::Destructor>;
}

%include "crystal/FFTIndexing.h"
%include "crystal/GruberReduction.h"

%include "instrument/DiffractometerStore.h"

%include "data/MetaData.h"
%include "data/IDataReader.h"

%ignore SX::Data::ILLDataReader::getData(size_t);
%include "data/ILLDataReader.h"
%extend SX::Data::ILLDataReader {
    std::vector<std::vector<int>> getData(unsigned int frame) 
    {
        Eigen::MatrixXi data = ($self)->getData(frame);
        int nrows = data.rows();
        int ncols = data.cols();
        std::vector<std::vector<int> > result(nrows);
	
        for (int i = 0; i < nrows; ++i) {
            result[i].reserve(ncols);
            for (int j = 0; j < ncols; ++j) {
                result[i].push_back(data(i, j));
            }
        }
	    return result;
    }
};

