%module "pynsx"

%include "warnings.i"

%include "typemaps.i"
%include "cpointer.i"

%include "std_shared_ptr.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_set.i"

%template(vector_1d)  std::vector<double>;
%template(vector_2d)  std::vector<std::vector<double>>;
%template(vector_1i) std::vector<int>;
%template(vector_2i) std::vector<std::vector<int>>;
%template(vector_string) std::vector<std::string>;

%shared_ptr(nsx::Peak3D)
%shared_ptr(nsx::Material)
%shared_ptr(nsx::Diffractometer)
%shared_ptr(nsx::ConvolutionKernel)
%shared_ptr(nsx::DeltaKernel)
%shared_ptr(nsx::AnnularKernel)
%shared_ptr(nsx::Convolver)
%shared_ptr(nsx::DataSet)
%shared_ptr(nsx::Source)

%{
#define SWIG_FILE_WITH_INIT

#include <Python.h>
#include <numpy/arrayobject.h>

#include <boost/property_tree/ptree.hpp>
 
#include <memory>

#include <Eigen/Core>
#include <Eigen/Geometry>

    using Eigen::RowVector2d;
    using Eigen::Vector2d;
    using Eigen::RowVector3d;
    using Eigen::Vector3d;
    using Eigen::Matrix3d;
    using Eigen::Matrix;
    using Eigen::Quaterniond;

namespace proptree=boost::property_tree;
namespace property_tree=boost::property_tree;

//#include "chemistry/ChemicalFormulaParser.h"
#include "chemistry/Material.h"
#include "chemistry/IsotopeDatabaseManager.h"

#include "mathematics/ErfInv.h"
#include "mathematics/Gaussian.h"
#include "mathematics/GCD.h"
#include "mathematics/Interpolator.h"
#include "mathematics/Lorentzian.h"
#include "mathematics/Minimizer.h"
#include "mathematics/Round.h"
#include "mathematics/RNG.h"

#include "utils/EigenToVector.h"
#include "utils/Types.h"
//#include "utils/ComplexParser.h"
#include "utils/System.h"
#include "utils/CSV.h"
#include "utils/Enums.h"
#include "utils/Units.h"
//#include "utils/LatticeConstraintParser.h"

#include "utils/DoubleToFraction.h"
#include "utils/EigenMatrixOp.h"
#include "utils/Timer.h"
#include "utils/Path.h"
#include "utils/Maybe.h"

#include "utils/Packer.h"
//#include "utils/AffineTransformParser.h"
#include "utils/LMFunctor.h"
#include "utils/RandomMatrix.h"
#include "utils/Parser.h"
#include "utils/YAMLType.h"
#include "utils/MatrixParser.h"
#include "utils/EigenMatrixParser.h"
#include "utils/ProgressHandler.h"

#include "crystal/PeakPredictor.h"
#include "crystal/UBMinimizer.h"
#include "crystal/SpaceGroupSymbols.h"
#include "crystal/Indexer.h"
#include "crystal/AutoIndexer.h"
#include "crystal/Profile.h"
#include "crystal/Intensity.h"
#include "crystal/LatticeFinder.h"
#include "crystal/PeakIntegrator.h"
#include "crystal/LatticeFunctor.h"
#include "crystal/UnitCell.h"
#include "crystal/ResolutionShell.h"
#include "crystal/RFactor.h"
#include "crystal/Peak3D.h"
#include "crystal/PeakCalc.h"

#include "crystal/PeakRecord.h"
#include "crystal/Mosaic.h"
#include "crystal/PeakRejector.h"
#include "crystal/LatticeMinimizer.h"
#include "crystal/PeakFit.h"
#include "crystal/FFTIndexing.h"
#include "crystal/LatticeSolution.h"

#include "crystal/MergedPeak.h"
#include "crystal/Peak2D.h"
#include "crystal/SpaceGroup.h"
#include "crystal/NiggliReduction.h"
#include "crystal/GruberReduction.h"
#include "crystal/SymOp.h"
#include "geometry/Vertex.h"
#include "geometry/Blob2D.h"
#include "geometry/Sphere.h"
#include "geometry/Triangle.h"

#include "geometry/IShape.h"
#include "geometry/Basis.h"
#include "geometry/OBB.h"
#include "geometry/Ellipsoid.h"
#include "geometry/Face.h"
#include "geometry/MCAbsorption.h"
#include "geometry/ShapeUnion.h"
#include "geometry/NDTree.h"
#include "geometry/ConvexHull.h"

#include "geometry/BlobFinder.h"
#include "geometry/Cluster.h"

#include "geometry/AABB.h"
#include "geometry/Edge.h"
#include "geometry/Blob3D.h"
#include "geometry/IntegrationRegion.h"

#include "data/XDS.h"
#include "data/DataSet.h"
#include "data/MetaData.h"
#include "data/ILLDataReader.h"
#include "data/IDataReader.h"
#include "data/HDF5DataReader.h"
#include "data/I16DataReader.h"
#include "data/BasicFrameIterator.h"
#include "data/RawDataReader.h"
#include "data/ThreadedFrameIterator.h"
#include "data/IFrameIterator.h"
#include "data/TiffDataReader.h"
#include "data/blosc_filter.h"
#include "data/PeakFinder.h"
#include "data/DataReaderFactory.h"
#include "instrument/Detector.h"
#include "instrument/DetectorFactory.h"
#include "instrument/TransAxis.h"
#include "instrument/Sample.h"
#include "instrument/ComponentState.h"
#include "instrument/DiffractometerStore.h"
#include "instrument/FlatDetector.h"
#include "instrument/Source.h"
#include "instrument/InstrumentState.h"
#include "instrument/MultiDetector.h"
#include "instrument/Monochromator.h"
#include "instrument/MonoDetector.h"
#include "instrument/Diffractometer.h"
#include "instrument/CylindricalDetector.h"
#include "instrument/Gonio.h"
#include "instrument/DetectorEvent.h"
#include "instrument/Axis.h"
#include "instrument/Experiment.h"
#include "instrument/DetectorState.h"

#include "instrument/Component.h"
#include "instrument/AxisFactory.h"
#include "instrument/RotAxis.h"
#include "physics/PhysicalUnit.h"
#include "imaging/ConstantKernel.h"
#include "imaging/KernelFactory.h"
#include "imaging/Convolver.h"

#include "imaging/DeltaKernel.h"
#include "imaging/AnnularKernel.h"
#include "imaging/ConvolutionKernel.h"

#include "kernel/Composite.h"
#include "kernel/Store.h"

#include "kernel/Store.h"
#include "kernel/Memento.h"
#include "kernel/Singleton.h"
#include "kernel/Factory.h"
#include "kernel/SharedFactory.h"

#include "utils/Enums.h"

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

using sptrMaterial = std::shared_ptr<nsx::Material>;
 
#include "instrument/Sample.h"


#include "geometry/Basis.h"

#include "geometry/IShape.h"
#include "geometry/AABB.h"

#include "geometry/Ellipsoid.h"
#include "geometry/OBB.h"
#include "geometry/Sphere.h"

#include "geometry/Blob3D.h"

#include "crystal/FFTIndexing.h"
#include "crystal/GruberReduction.h"
#include "crystal/Peak3D.h"
#include "crystal/SpaceGroup.h"

#include "crystal/UnitCell.h"
#include "crystal/PeakIntegrator.h"
#include "crystal/Profile.h"
#include "crystal/Intensity.h"

using sptrUnitCell = std::shared_ptr<nsx::UnitCell>;

#include "instrument/Diffractometer.h"
#include "instrument/DiffractometerStore.h"

#include "kernel/Singleton.h"


#include "data/MetaData.h"
#include "data/IDataReader.h"
#include "data/DataReaderFactory.h"
#include "data/ILLDataReader.h"
#include "data/HDF5DataReader.h"

#include "data/DataSet.h"
#include "data/PeakFinder.h"

using namespace nsx;

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

 //%include <boost/property_tree/ptree.hpp>
namespace boost { namespace property_tree {} }

%include "instrument/Axis.h"
%include "instrument/RotAxis.h"
%include "instrument/TransAxis.h"
%include "instrument/Gonio.h"

%include "instrument/ComponentState.h"
%include "instrument/Component.h"
%include "instrument/Monochromator.h"
%include "instrument/Source.h"

%include "geometry/IShape.h"
%template(IShape3D) nsx::IShape<double,3>;

%typemap(out) AABB<double, 3>::vector = Eigen::Vector3d;
%typemap(out) Ellipsoid<double, 3>::vector = Eigen::Vector3d;
%typemap(out) OBB<double, 3>::vector = Eigen::Vector3d;
%typemap(out) Sphere<double, 3>::vector = Eigen::Vector3d;


%include "geometry/AABB.h"
%template(AABB3D) nsx::AABB<double,3>;

%include "geometry/Sphere.h"

%include "geometry/Ellipsoid.h"
%template(Ellipsoid3D) nsx::Ellipsoid<double,3>;
%include "geometry/OBB.h"
%template(OBB3D) nsx::OBB<double,3>;
%include "geometry/Sphere.h"

%template(Sphere3D) nsx::Sphere<double,3>;

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
 //%include "kernel/Factory.h"

namespace nsx {
   class DiffractometerStore;
   class DataReaderFactory; 
   struct tVector;
   %template(DiffractometerStoreBase) Singleton<DiffractometerStore, Constructor, Destructor>;
    %template(DataReaderFactorySingletonBase) Singleton<DataReaderFactory, Constructor, Destructor>;
    //  %template(DataReaderFactoryFactoryBase) Factory<DataSet,std::string,std::string,std::shared_ptr<Diffractometer> >;
}

%include "crystal/FFTIndexing.h"
%include "crystal/GruberReduction.h"
%include "crystal/Intensity.h"
%include "crystal/Peak3D.h"

%include "instrument/DiffractometerStore.h"

%include "imaging/ConstantKernel.h"
%include "imaging/KernelFactory.h"
%include "imaging/ConvolutionKernel.h"
%include "imaging/DeltaKernel.h"
%include "imaging/AnnularKernel.h"
%include "imaging/Convolver.h"

%include "data/MetaData.h"
%include "data/IDataReader.h"

%include "data/DataReaderFactory.h"

%include "data/ILLDataReader.h"
%include "data/HDF5DataReader.h"

%include "data/DataSet.h"
%include "data/PeakFinder.h"

%template(vector_data) std::vector<std::shared_ptr<nsx::DataSet>>;
%template(vector_peak) std::vector<std::shared_ptr<nsx::Peak3D>>;
%template(set_peak) std::set<std::shared_ptr<nsx::Peak3D>>;




 //%include "chemistry/ChemicalFormulaParser.h"
%include "chemistry/Material.h"
 //%include "chemistry/IsotopeDatabaseManager.h"

%include "mathematics/ErfInv.h"
%include "mathematics/Gaussian.h"
%include "mathematics/GCD.h"
%include "mathematics/Interpolator.h"
%include "mathematics/Lorentzian.h"
%include "mathematics/Round.h"
%include "mathematics/Minimizer.h"
%include "mathematics/RNG.h"

%include "utils/EigenToVector.h"
%include "utils/Types.h"
 //%include "utils/ComplexParser.h"
%include "utils/System.h"
%include "utils/CSV.h"
%include "utils/Enums.h"
 //%include "utils/Units.h"
 //%include "utils/LatticeConstraintParser.h"
%include "utils/DoubleToFraction.h"
%include "utils/EigenMatrixOp.h"
%include "utils/Timer.h"
%include "utils/Path.h"
 //%include "utils/Maybe.h"
 //%include "utils/Packer.h"
 //%include "utils/AffineTransformParser.h"
%include "utils/LMFunctor.h"
%include "utils/RandomMatrix.h"
%include "utils/Parser.h"
 //%include "utils/YAMLType.h"
%include "utils/MatrixParser.h"
 //%include "utils/EigenMatrixParser.h"
%include "utils/ProgressHandler.h"
%include "crystal/PeakPredictor.h"
%include "crystal/UBMinimizer.h"
%include "crystal/SpaceGroupSymbols.h"
%include "crystal/Indexer.h"
 //%include "crystal/AutoIndexer.h"
%include "crystal/Profile.h"
%include "crystal/Intensity.h"
%include "crystal/LatticeFinder.h"
%include "crystal/PeakIntegrator.h"
%include "crystal/LatticeFunctor.h"
%include "crystal/UnitCell.h"
%include "crystal/ResolutionShell.h"
%include "crystal/RFactor.h"
%include "crystal/Peak3D.h"
%include "crystal/PeakCalc.h"
%include "crystal/PeakRecord.h"
%include "crystal/Mosaic.h"
%include "crystal/PeakRejector.h"
%include "crystal/LatticeMinimizer.h"
%include "crystal/PeakFit.h"
%include "crystal/FFTIndexing.h"
%include "crystal/LatticeSolution.h"
%include "crystal/MergedPeak.h"
%include "crystal/Peak2D.h"
%include "crystal/SpaceGroup.h"
%include "crystal/NiggliReduction.h"
%include "crystal/GruberReduction.h"
%include "crystal/SymOp.h"
%include "geometry/Vertex.h"
%include "geometry/Blob2D.h"
%include "geometry/Sphere.h"
%include "geometry/Triangle.h"

%include "geometry/IShape.h"
%include "geometry/Basis.h"
%include "geometry/OBB.h"
%include "geometry/Ellipsoid.h"
%include "geometry/Face.h"
%include "geometry/MCAbsorption.h"
%include "geometry/ShapeUnion.h"
%include "geometry/NDTree.h"
%include "geometry/ConvexHull.h"
%include "geometry/BlobFinder.h"
%include "geometry/Cluster.h"
%include "geometry/AABB.h"
%include "geometry/Edge.h"
%include "geometry/Blob3D.h"
%include "geometry/IntegrationRegion.h"
%include "data/XDS.h"
%include "data/DataSet.h"
%include "data/MetaData.h"
%include "data/ILLDataReader.h"
%include "data/IDataReader.h"
%include "data/HDF5DataReader.h"
%include "data/I16DataReader.h"
 //%include "data/BasicFrameIterator.h"
%include "data/RawDataReader.h"
 //%include "data/ThreadedFrameIterator.h"
 //%include "data/IFrameIterator.h"
%include "data/TiffDataReader.h"
%include "data/blosc_filter.h"


%include "data/DataReaderFactory.h"
%include "instrument/Detector.h"
%include "instrument/DetectorFactory.h"
%include "instrument/TransAxis.h"
%include "instrument/Sample.h"
%include "instrument/ComponentState.h"
%include "instrument/DiffractometerStore.h"
%include "instrument/FlatDetector.h"
%include "instrument/Source.h"
%include "instrument/InstrumentState.h"
%include "instrument/MultiDetector.h"
%include "instrument/Monochromator.h"
%include "instrument/MonoDetector.h"
%include "instrument/Diffractometer.h"
%include "instrument/CylindricalDetector.h"
%include "instrument/Gonio.h"
%include "instrument/DetectorEvent.h"
%include "instrument/Axis.h"
%include "instrument/Experiment.h"
%include "instrument/DetectorState.h"

%include "instrument/Component.h"
%include "instrument/AxisFactory.h"
%include "instrument/RotAxis.h"
 //%include "physics/PhysicalUnit.h"

 //%include "kernel/Composite.h"
%include "kernel/Store.h"
 //%include "kernel/State.h"
%include "kernel/Memento.h"
%include "kernel/Singleton.h"
 //%include "kernel/Factory.h"
 //%include "kernel/SharedFactory.h"



 //%ignore nsx::getData(size_t);
 //%include "data/ILLDataReader.h"



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


