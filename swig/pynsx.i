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
#include "utils/sptr.h"
#include "utils/MinimizerGSL.h"
#include "utils/EigenToVector.h"
#include "utils/Types.h"
#include "utils/ComplexParser.h"
#include "utils/System.h"
#include "utils/CSV.h"
#include "utils/Enums.h"
#include "utils/Units.h"
#include "utils/LatticeConstraintParser.h"
#include "utils/DoubleToFraction.h"
#include "utils/Gaussian.h"
#include "utils/EigenMatrixOp.h"
#include "utils/Timer.h"
#include "utils/Path.h"
#include "utils/Maybe.h"
#include "utils/Packer.h"
//#include "utils/AffineTransformParser.h"
#include "utils/Lorentzian.h"
#include "utils/IMinimizer.h"
#include "utils/Round.h"
#include "utils/erf_inv.h"
#include "utils/RNG.h"
#include "utils/LMFunctor.h"
#include "utils/RandomMatrix.h"
#include "utils/Parser.h"
#include "utils/YAMLType.h"
#include "utils/gcd.h"
#include "utils/MatrixParser.h"
#include "utils/EigenMatrixParser.h"
#include "utils/ProgressHandler.h"
#include "utils/Interpolator.h"
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
using SX::Crystal::LatticeSolution;
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
using SX::Geometry::ConvexHull;
#include "geometry/BlobFinder.h"
#include "geometry/Cluster.h"
#include "geometry/AABB.h"
#include "geometry/Edge.h"
#include "geometry/Blob3D.h"
#include "geometry/IntegrationRegion.h"
#include "data/XDS.h"
#include "data/IData.h"
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
using SX::Instrument::DetectorState;
#include "instrument/Component.h"
#include "instrument/AxisFactory.h"
#include "instrument/RotAxis.h"
#include "physics/PhysicalUnit.h"
#include "imaging/FrameFilter.h"
#include "imaging/ConstantKernel.h"
#include "imaging/KernelFactory.h"
#include "imaging/Convolver.h"
#include "imaging/DeltaKernel.h"
#include "imaging/AnnularKernel.h"
#include "imaging/ConvolutionKernel.h"
 using SX::Imaging::ConvolutionKernel;
#include "kernel/Error.h"
#include "kernel/Composite.h"
#include "kernel/Store.h"

#include "kernel/Store.h"
#include "kernel/Memento.h"
#include "kernel/Singleton.h"
#include "kernel/Factory.h"
#include "kernel/SharedFactory.h"



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
using SX::Geometry::AABB;
#include "geometry/Ellipsoid.h"
#include "geometry/OBB.h"
#include "geometry/Sphere.h"

#include "geometry/Blob3D.h"

#include "crystal/FFTIndexing.h"
#include "crystal/GruberReduction.h"
#include "crystal/Peak3D.h"
#include "crystal/SpaceGroup.h"
using SX::Crystal::SpaceGroup;
#include "crystal/UnitCell.h"
#include "crystal/PeakIntegrator.h"
#include "crystal/Profile.h"
#include "crystal/Intensity.h"
using SX::Crystal::Profile;
using SX::Crystal::Intensity; 

using sptrUnitCell = std::shared_ptr<SX::Crystal::UnitCell>;

#include "instrument/Diffractometer.h"
#include "instrument/DiffractometerStore.h"

#include "kernel/Singleton.h"


#include "data/MetaData.h"
#include "data/IDataReader.h"
#include "data/DataReaderFactory.h"
#include "data/ILLDataReader.h"
#include "data/HDF5DataReader.h"

#include "data/IData.h"
#include "data/PeakFinder.h"

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
   namespace Data { class DataReaderFactory; }
   namespace Crystal {struct tVector;}
   %template(DiffractometerStoreBase) Kernel::Singleton<Instrument::DiffractometerStore, Kernel::Constructor, Kernel::Destructor>;
    %template(DataReaderFactorySingletonBase) Kernel::Singleton<Data::DataReaderFactory, Kernel::Constructor, Kernel::Destructor>;
}

%include "crystal/FFTIndexing.h"
%include "crystal/GruberReduction.h"
%include "crystal/Peak3D.h"

%include "instrument/DiffractometerStore.h"

%include "data/MetaData.h"
%include "data/IDataReader.h"

%include "data/DataReaderFactory.h"

%include "data/ILLDataReader.h"
%include "data/HDF5DataReader.h"

%include "data/IData.h"
%include "data/PeakFinder.h"





 //%include "chemistry/ChemicalFormulaParser.h"
%include "chemistry/Material.h"
 //%include "chemistry/IsotopeDatabaseManager.h"
%include "utils/sptr.h"
%include "utils/MinimizerGSL.h"
%include "utils/EigenToVector.h"
%include "utils/Types.h"
%include "utils/ComplexParser.h"
%include "utils/System.h"
%include "utils/CSV.h"
%include "utils/Enums.h"
 //%include "utils/Units.h"
 //%include "utils/LatticeConstraintParser.h"
%include "utils/DoubleToFraction.h"
%include "utils/Gaussian.h"
%include "utils/EigenMatrixOp.h"
%include "utils/Timer.h"
%include "utils/Path.h"
 //%include "utils/Maybe.h"
 //%include "utils/Packer.h"
 //%include "utils/AffineTransformParser.h"
%include "utils/Lorentzian.h"
%include "utils/IMinimizer.h"
%include "utils/Round.h"
%include "utils/erf_inv.h"
%include "utils/RNG.h"
%include "utils/LMFunctor.h"
%include "utils/RandomMatrix.h"
%include "utils/Parser.h"
 //%include "utils/YAMLType.h"
%include "utils/gcd.h"
%include "utils/MatrixParser.h"
 //%include "utils/EigenMatrixParser.h"
%include "utils/ProgressHandler.h"
%include "utils/Interpolator.h"
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
%include "data/IData.h"
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
%include "data/PeakFinder.h"
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
using SX::Instrument::DetectorState;
%include "instrument/Component.h"
%include "instrument/AxisFactory.h"
%include "instrument/RotAxis.h"
 //%include "physics/PhysicalUnit.h"
%include "imaging/FrameFilter.h"
%include "imaging/ConstantKernel.h"
%include "imaging/KernelFactory.h"
%include "imaging/Convolver.h"
%include "imaging/DeltaKernel.h"
%include "imaging/AnnularKernel.h"
%include "imaging/ConvolutionKernel.h"
%include "kernel/Error.h"
 //%include "kernel/Composite.h"
%include "kernel/Store.h"
 //%include "kernel/State.h"
%include "kernel/Memento.h"
%include "kernel/Singleton.h"
 //%include "kernel/Factory.h"
 //%include "kernel/SharedFactory.h"



 //%ignore SX::Data::ILLDataReader::getData(size_t);
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

