%module(directors="1") "pynsx"

%include "exception.i"

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  }
}

%feature("director") IPeakIntegrator;
%feature("director") MeanBackgroundIntegrator;
%feature("director") PixelSumIntegrator;
%feature("director") Profile3DIntegrator;
%feature("director") ISigmaIntegrator;
%feature("director") ShapeIntegrator;

%include "pynsx_doc.i"
%include "warnings.i"
%include "typemaps.i"
%include "cpointer.i"
%include "std_shared_ptr.i"
%include "std_string.i"
%include "std_vector.i"
%include "std_deque.i"
%include "std_set.i"
%include "std_pair.i"

%template(vector_1d)  std::vector<double>;
%template(vector_2d)  std::vector<std::vector<double>>;
%template(vector_1i) std::vector<int>;
%template(vector_2i) std::vector<std::vector<int>>;
%template(vector_string) std::vector<std::string>;

%shared_ptr(nsx::Peak3D)
%shared_ptr(nsx::Diffractometer)
%shared_ptr(nsx::DataSet)
%shared_ptr(nsx::IDataReader)
%shared_ptr(nsx::HDF5DataReader)
%shared_ptr(nsx::HDF5MetaDataReader)
%shared_ptr(nsx::ILLDataReader)
%shared_ptr(nsx::RawDataReader)
%shared_ptr(nsx::TiffDataReader)
%shared_ptr(nsx::Experiment)
%shared_ptr(nsx::ProgressHandler)
%shared_ptr(nsx::UnitCell)
%shared_ptr(nsx::ShapeLibrary)

%{
#pragma GCC diagnostic ignored "-Wpedantic"
#define SWIG_FILE_WITH_INIT
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include <Python.h>
#include <numpy/arrayobject.h>
#include <memory>
#include <Eigen/Core>
#include <Eigen/Geometry>

using Eigen::RowVector2d;
using Eigen::Vector2d;
using Eigen::RowVector3d;
using Eigen::Vector3d;
using Eigen::RowVector3i;
using Eigen::Vector3i;
using Eigen::Matrix3d;
using Eigen::Matrix;
using Eigen::Quaterniond;

#include "Version.h"
#include "Any.h"
#include "Material.h"
#include "IsotopeDatabaseManager.h"
#include "FitParameters.h"
#include "Minimizer.h"
#include "EigenToVector.h"
#include "CSV.h"
#include "DataOrder.h"
#include "Units.h"
#include "DoubleToFraction.h"
#include "Path.h"
#include "Parser.h"
#include "YAMLType.h"
#include "MatrixParser.h"
#include "ProgressHandler.h"
#include "PeakCoordinateSystem.h"
#include "RefinementBatch.h"
#include "Refiner.h"
#include "AutoIndexer.h"
#include "Intensity.h"
#include "IPeakIntegrator.h"
#include "MeanBackgroundIntegrator.h"
#include "Profile3DIntegrator.h"
#include "ISigmaIntegrator.h"
#include "PixelSumIntegrator.h"
#include "ShapeIntegrator.h"
#include "IntegrationRegion.h"
#include "PeakData.h"
#include "UnitCell.h"
#include "ResolutionShell.h"
#include "RFactor.h"
#include "CC.h"
#include "Peak3D.h"
using sptrPeak3D = std::shared_ptr<nsx::Peak3D>;
#include "FFTIndexing.h"
#include "MergedPeak.h"
#include "SpaceGroup.h"
#include "NiggliReduction.h"
#include "GruberReduction.h"
#include "SymOp.h"
#include "Vertex.h"
#include "Triangle.h"
#include "Ellipsoid.h"
#include "Face.h"
#include "MCAbsorption.h"
#include "ConvexHull.h"
#include "BrillouinZone.h"
#include "AABB.h"
#include "Edge.h"
#include "Blob3D.h"
#include "DirectVector.h"
#include "ReciprocalVector.h"
#include "DetectorEvent.h"
#include "DataSet.h"
#include "Profile3D.h"
#include "MetaData.h"
#include "ILLDataReader.h"
#include "IDataReader.h"
#include "HDF5DataReader.h"
#include "HDF5MetaDataReader.h"
#include "RawDataReader.h"
#include "TiffDataReader.h"
#include "BloscFilter.h"
#include "ITask.h"
#include "PeakFinder.h"
#include "Detector.h"
#include "DetectorFactory.h"
#include "TransAxis.h"
#include "Sample.h"
#include "FlatDetector.h"
#include "Source.h"
#include "InstrumentState.h"
#include "InterpolatedState.h"
#include "Monochromator.h"
#include "Diffractometer.h"
#include "CylindricalDetector.h"
#include "Gonio.h"
#include "Convolver.h"
#include "AtomicConvolver.h"
#include "DeltaConvolver.h"
#include "AnnularConvolver.h"
#include "EnhancedAnnularConvolver.h"
#include "ConstantConvolver.h"
#include "ConvolverFactory.h"
#include "RadialConvolver.h"
#include "mosaic.h"
#include "Axis.h"
#include "Experiment.h"
#include "Component.h"
#include "AxisFactory.h"
#include "RotAxis.h"
#include "Singleton.h"
#include "Factory.h"
#include "DataOrder.h"
#include "Axis.h"
#include "RotAxis.h"
#include "TransAxis.h"
#include "Monochromator.h"
#include "Detector.h"
#include "CylindricalDetector.h"
#include "FlatDetector.h"
#include "Source.h"
#include "Sample.h"
#include "AABB.h"
#include "Ellipsoid.h"
#include "Blob3D.h"
#include "FFTIndexing.h"
#include "Peak3D.h"
#include "SpaceGroup.h"
#include "UnitCell.h"
#include "GruberReduction.h"
#include "Intensity.h"
using sptrUnitCell = std::shared_ptr<nsx::UnitCell>;
#include "Diffractometer.h"
using sptrDiffractometer = std::shared_ptr<nsx::Diffractometer>;
#include "Singleton.h"
#include "Convolver.h"
#include "AtomicConvolver.h"
#include "ConvolverFactory.h"
#include "DeltaConvolver.h"
#include "AnnularConvolver.h"
#include "EnhancedAnnularConvolver.h"
#include "ConstantConvolver.h"
#include "RadialConvolver.h"
#include "MetaData.h"
#include "IDataReader.h"
#include "ILLDataReader.h"
#include "HDF5DataReader.h"
#include "DataSet.h"
#include "MergedData.h"
#include "PeakFilter.h"
#include "MillerIndex.h"
#include "Profile1D.h"
#include "ShapeLibrary.h"
using sptrShapeLibrary = std::shared_ptr<nsx::ShapeLibrary>;
#include "CC.h"
#include "RFactor.h"

using namespace nsx;

%}

%include "numpy.i"
%include "eigen.i"

%init %{
    import_array();
%}

%include <typemaps.i>
%include <std_vector.i>
%include <std_map.i>

// eigen.i is found in ../swig/ and contains specific definitions to convert
// Eigen matrices into Numpy arrays.
%include <eigen.i>

// Since Eigen uses templates, we have to declare exactly which types we'd
// like to generate mappings for
//%eigen_typemaps(Eigen::Vector3i)
%eigen_typemaps(Eigen::Vector3d)
%eigen_typemaps(Eigen::Vector4d)
%eigen_typemaps(Eigen::RowVector3i)
%eigen_typemaps(Eigen::RowVector3d)
%eigen_typemaps(Eigen::RowVector4d)
%eigen_typemaps(Eigen::Matrix3d)
%eigen_typemaps(Eigen::VectorXd)
%eigen_typemaps(Eigen::MatrixXd)
%eigen_typemaps(Eigen::MatrixXi)
%eigen_typemaps(Eigen::ArrayXd)
%eigen_typemaps(Eigen::ArrayXXd)
// Even though Eigen::MatrixXd is just a typedef for Eigen::Matrix<double,
// Eigen::Dynamic, Eigen::Dynamic>, our templatedInverse function doesn't
// compile correctly unless we also declare typemaps for Eigen::Matrix<double,
// Eigen::Dynamic, Eigen::Dynamic>. Not totally sure why that is.
%eigen_typemaps(Eigen::Matrix<double, 3, 1>)
%eigen_typemaps(Eigen::Matrix<double, 1, 3>)
%eigen_typemaps(Eigen::Matrix<double, 4, 1>)
%eigen_typemaps(Eigen::Matrix<double, 1, 4>)
%eigen_typemaps(Eigen::Matrix<int, 3, 1>)
%eigen_typemaps(Eigen::Matrix<int, 1, 3>)
%eigen_typemaps(Eigen::Matrix<double, 3, 3>)
%eigen_typemaps(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>)
%eigen_typemaps(Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic>)
%eigen_typemaps(Eigen::Array<double, Eigen::Dynamic, 1>)
%eigen_typemaps(Eigen::Array<double, Eigen::Dynamic, Eigen::Dynamic>)
%eigen_typemaps(Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>)

%template(vectorMatrixXd) std::vector<Eigen::MatrixXd>;
%template(vectorVectorXd) std::vector<Eigen::VectorXd>;
%template(vectorVector3d) std::vector<Eigen::Vector3d>;
%template(vectorRowVector3d) std::vector<Eigen::RowVector3d>;
%include "Version.h"
%include "ChemistryTypes.h"
%include "PeakList.h"
%include "DataTypes.h"
%include "GeometryTypes.h"
%include "RealMatrix.h"
%include "UnitCell.h"
%template(UnitCellList) std::vector<nsx::sptrUnitCell>;
%include "DirectVector.h"
%template(DirectVectorList) std::vector<nsx::DirectVector>;
%include "ReciprocalVector.h"
%template(ReciprocalVectorList) std::vector<nsx::ReciprocalVector>;
%template(ReciprocalVectorQueue) std::deque<nsx::ReciprocalVector>;
%include "Axis.h"
%include "RotAxis.h"
%include "TransAxis.h"
%include "Gonio.h"
%include "Component.h"
%include "Monochromator.h"
%include "Source.h"
%include "AABB.h"
%include "Ellipsoid.h"
%include "Blob3D.h"

%include "Any.h"
%extend nsx::Any {
     %template(Anyi) Any<int>;
     %template(Anyd) Any<double>;
     %template(Anyb) Any<bool>;
     %template(Anys) Any<std::string>;
}

%include "Material.h"
%include "IsotopeDatabaseManager.h"
%template(propertyi) nsx::IsotopeDatabaseManager::property<int>;
%template(propertyd) nsx::IsotopeDatabaseManager::property<double>;
%template(propertys) nsx::IsotopeDatabaseManager::property<std::string>;
%template(propertyb) nsx::IsotopeDatabaseManager::property<bool>;
%template(scored_uc) std::pair<std::shared_ptr<nsx::UnitCell>, double>;
%template(indexer_solutions) std::vector<std::pair<std::shared_ptr<nsx::UnitCell>,double>>;
%include "Detector.h"
%include "DetectorEvent.h"
%template(DetectorEventQueue) std::deque<nsx::DetectorEvent>;
%template(DetectorEventList) std::vector<nsx::DetectorEvent>;
%template(DoubleQueue) std::deque<double>;
%include "CylindricalDetector.h"
%include "FlatDetector.h"
%include "Diffractometer.h"
%include "Sample.h"
%include "Singleton.h"
%include "InstrumentState.h"
%include "InterpolatedState.h"

namespace nsx {
   class ConvolverFactory;
   struct tVector;
}

%include "FFTIndexing.h"
%include "PeakData.h"
%include "IntegrationRegion.h"
%include "Intensity.h"
%template(IntensityList) std::vector<nsx::Intensity>;
%include "Profile3D.h"
%include "Profile1D.h"
%include "IPeakIntegrator.h"
%include "MeanBackgroundIntegrator.h"
%include "Profile3DIntegrator.h"
%include "ISigmaIntegrator.h"
%include "PixelSumIntegrator.h"
%include "Peak3D.h"
%include "Profile1D.h"
%include "ShapeLibrary.h"
%include "ShapeIntegrator.h"
%include "IntegrationRegion.h"
%include "PeakData.h"
%include "Intensity.h"
%include "Peak3D.h"
%template(PeakList) std::vector<std::shared_ptr<nsx::Peak3D>>;
%template(PeakShell) std::vector<std::vector<std::shared_ptr<nsx::Peak3D>>>;
%include "PeakFilter.h"
%template(ConvolverParameters) std::map<std::string,double>;
%include "Convolver.h"
%include "ConvolverFactory.h"
%include "ConstantConvolver.h"
%include "DeltaConvolver.h"
%include "AnnularConvolver.h"
%include "EnhancedAnnularConvolver.h"
%include "RadialConvolver.h"
%include "Convolver.h"
%include "AtomicConvolver.h"
%include "mosaic.h"
%include "MetaData.h"
%include "IDataReader.h"
%include "ILLDataReader.h"
%include "HDF5MetaDataReader.h"
%include "HDF5DataReader.h"
%include "Profile3D.h"
%include "DataSet.h"
%include "MergedData.h"
%template(DataList) std::vector<std::shared_ptr<nsx::DataSet>>;
%include "ITask.h"
%include "PeakFinder.h"
%template(MergedPeakSet) std::set<nsx::MergedPeak>;
%include "FitParameters.h"
%include "Minimizer.h"
%include "EigenToVector.h"
%include "CSV.h"
%include "DataOrder.h"
%include "DoubleToFraction.h"
%include "Path.h"
%include "Parser.h"
%include "MatrixParser.h"
%include "ProgressHandler.h"
%include "PeakCoordinateSystem.h"
%include "RefinementBatch.h"
%include "Refiner.h"
%template(RefinementBatchList) std::vector<nsx::RefinementBatch>;
%include "Intensity.h"
%include "IntegrationRegion.h"
%include "PeakData.h"
%include "UnitCell.h"
%include "ResolutionShell.h"
%include "RFactor.h"
%include "CC.h"
%include "CC.h"
%include "Peak3D.h"
%include "MillerIndex.h"
%template(MillerIndexList) std::vector<nsx::MillerIndex>;
%include "Profile1D.h"
%include "ShapeLibrary.h"
%include "FFTIndexing.h"
%include "MergedPeak.h"
%include "SpaceGroup.h"
%include "NiggliReduction.h"
%include "GruberReduction.h"
%include "SymOp.h"
%template(SymOpList) std::vector<nsx::SymOp>;
%include "Vertex.h"
%include "Triangle.h"
%include "Ellipsoid.h"
%include "Face.h"
%include "MCAbsorption.h"
%include "ConvexHull.h"
%include "AABB.h"
%include "Edge.h"
%include "Blob3D.h"
%include "BrillouinZone.h"
%include "Profile3D.h"
%include "DataSet.h"
%include "MetaData.h"
%include "ILLDataReader.h"
%include "IDataReader.h"
%include "HDF5MetaDataReader.h"
%include "HDF5DataReader.h"
%include "RawDataReader.h"
%include "TiffDataReader.h"
%include "BloscFilter.h"
%include "Convolver.h"
%include "AtomicConvolver.h"
%include "ConstantConvolver.h"
%include "DeltaConvolver.h"
%include "AnnularConvolver.h"
%include "EnhancedAnnularConvolver.h"
%include "RadialConvolver.h"
%include "ConvolverFactory.h"
%include "Detector.h"
%include "DetectorFactory.h"
%include "TransAxis.h"
%include "Sample.h"
%include "FlatDetector.h"
%include "Source.h"
%include "InstrumentState.h"
%include "InterpolatedState.h"
%include "Monochromator.h"
%include "Diffractometer.h"
%include "CylindricalDetector.h"
%template(InstrumentStateList) std::vector<nsx::InstrumentState>;
%include "Axis.h"
%include "Experiment.h"
%include "AxisFactory.h"
%include "RotAxis.h"
%include "AutoIndexer.h"
%include "Singleton.h"

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
