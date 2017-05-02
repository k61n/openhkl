%module "pynsx"
 
%include "std_shared_ptr.i"
%include "std_string.i"
%include "std_vector.i"

%template(vdouble1d_t)  std::vector<double>;
%template(vdouble2d_t)  std::vector<std::vector<double>>;
%template(vinteger1d_t) std::vector<int>;
%template(vinteger2d_t) std::vector<std::vector<int>>;

%shared_ptr(SX::Crystal::Peak3D)
%shared_ptr(SX::Chemistry::Material)
%shared_ptr(SX::Instrument::Diffractometer)

%{
#define SWIG_FILE_WITH_INIT

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

%include <boost/property_tree/ptree.hpp>

%include "instrument/Axis.h"
%include "instrument/RotAxis.h"
%include "instrument/TransAxis.h"
%include "instrument/Gonio.h"

%include "instrument/ComponentState.h"
%include "instrument/Component.h"
%include "instrument/Monochromator.h"
%include "instrument/Source.h"

%ignore SX::Geometry::Basis::getMetricTensor() const;
%include "geometry/Basis.h"
%extend SX::Geometry::Basis {
    std::vector<std::vector<double>> getMetricTensor()
    {
        Eigen::Matrix3d metricTensor = ($self)->getMetricTensor();
        std::vector<std::vector<double> > result(3);
    
        for (int i = 0; i < 3; ++i) {
            result[i].reserve(3);
            for (int j = 0; j < 3; ++j) {
                result[i].push_back(metricTensor(i,j));
            }
        }
        return result;
    }
};

%ignore SX::Geometry::IShape::getLower();
%ignore SX::Geometry::IShape::getLower() const;
%include "geometry/IShape.h"

%extend SX::Geometry::IShape {
    std::vector<T> getLower() 
    {
        Eigen::Matrix<T,D,1> data = ($self)->getLower();
        std::vector<T> result;
        result.reserve(D);            
        for (int i = 0; i < D; ++i) {
            result.push_back(data(i));
        }
        return result;
    }
};

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

