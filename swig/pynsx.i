%module "pynsx"

%include "warnings.i"

%include "typemaps.i"
%include "cpointer.i"

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
 
#include <Eigen/Core>
#include <Eigen/Geometry>

    using Eigen::Vector3d;
    using Eigen::Matrix3d;
    using Eigen::Matrix;
    using Eigen::Quaterniond;

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
using SX::Chemistry::isotopeContents;
 
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

%include <typemaps.i>
%include <std_vector.i>

// eigen.i is found in ../swig/ and contains specific definitions to convert
// Eigen matrices into Numpy arrays.
%include <eigen.i>

%template(vectorMatrixXd) std::vector<Eigen::MatrixXd>;
%template(vectorVectorXd) std::vector<Eigen::VectorXd>;
%template(vectorVector3d) std::vector<Eigen::Vector3d>;

// Since Eigen uses templates, we have to declare exactly which types we'd
// like to generate mappings for
%eigen_typemaps(Eigen::Vector3d)
%eigen_typemaps(Eigen::Matrix3d)
%eigen_typemaps(Eigen::VectorXd)
%eigen_typemaps(Eigen::MatrixXd)
%eigen_typemaps(Eigen::MatrixXi)
// Even though Eigen::MatrixXd is just a typedef for Eigen::Matrix<double,
// Eigen::Dynamic, Eigen::Dynamic>, our templatedInverse function doesn't
// compile correctly unless we also declare typemaps for Eigen::Matrix<double,
// Eigen::Dynamic, Eigen::Dynamic>. Not totally sure why that is.
%eigen_typemaps(Eigen::Matrix<double, 3, 1>)
%eigen_typemaps(Eigen::Matrix<double, 1, 3>)
%eigen_typemaps(Eigen::Matrix<double, 3, 3>)
%eigen_typemaps(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>)
%eigen_typemaps(Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>)

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

%typemap(out) SX::Geometry::AABB<double, 3>::vector = Eigen::Vector3d;
%typemap(out) SX::Geometry::Ellipsoid<double, 3>::vector = Eigen::Vector3d;
%typemap(out) SX::Geometry::OBB<double, 3>::vector = Eigen::Vector3d;
%typemap(out) SX::Geometry::Sphere<double, 3>::vector = Eigen::Vector3d;

%include "geometry/AABB.h"
%template(AABB3D) SX::Geometry::AABB<double,3>;

%include "geometry/Sphere.h"

%include "geometry/Ellipsoid.h"
%template(Ellipsoid3D) SX::Geometry::Ellipsoid<double,3>;
%include "geometry/OBB.h"
%template(OBB3D) SX::Geometry::OBB<double,3>;
%include "geometry/Sphere.h"

%template(Sphere3D) SX::Geometry::Sphere<double,3>;

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

%include "data/ILLDataReader.h"

%newobject new_double;
double* new_double();
double get_value(const double*);
%{
    double* new_double()
    {
        return new double;
    }

    double get_value(const double* ptr)
    {
        return *ptr;
    }
%}

