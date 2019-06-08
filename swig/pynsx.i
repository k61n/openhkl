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
%feature("director") Profile1DIntegrator;
%feature("director") ISigmaIntegrator;
%feature("director") ShapeIntegrator;

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

#include "core/utils/Any.h"

#include "core/chemistry/Material.h"
#include "core/chemistry/IsotopeDatabaseManager.h"

#include "core/fitting/FitParameters.h"
#include "core/fitting/Minimizer.h"

#include "core/data/EigenToVector.h"
#include "core/utils/System.h"
#include "core/utils/CSV.h"
#include "core/utils/Enums.h"
#include "core/utils/Units.h"

#include "core/utils/DoubleToFraction.h"
#include "core/utils/Path.h"

#include "core/data/Parser.h"
#include "core/utils/YAMLType.h"
#include "core/data/MatrixParser.h"
#include "core/utils/ProgressHandler.h"

#include "core/crystal/PeakCoordinateSystem.h"
#include "core/refine/RefinementBatch.h"
#include "core/refine/Refiner.h"
#include "core/auto_indexing/AutoIndexer.h"
#include "core/crystal/Intensity.h"
#include "core/integration/IPeakIntegrator.h"
#include "core/integration/MeanBackgroundIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/integration/IntegrationRegion.h"
#include "core/crystal/PeakData.h"
#include "core/crystal/UnitCell.h"
#include "core/crystal/ResolutionShell.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/CC.h"
#include "core/crystal/Peak3D.h"
using sptrPeak3D = std::shared_ptr<nsx::Peak3D>;

#include "core/auto_indexing/FFTIndexing.h"

#include "core/crystal/MergedPeak.h"
#include "core/crystal/SpaceGroup.h"
#include "core/crystal/NiggliReduction.h"
#include "core/crystal/GruberReduction.h"
#include "core/crystal/SymOp.h"
#include "core/geometry/Vertex.h"
#include "core/geometry/Triangle.h"

#include "core/geometry/Ellipsoid.h"
#include "core/geometry/Face.h"
#include "core/monte-carlo/MCAbsorption.h"
#include "core/geometry/ConvexHull.h"
#include "core/crystal/BrillouinZone.h"

#include "core/geometry/AABB.h"
#include "core/geometry/Edge.h"
#include "core/geometry/Blob3D.h"


#include "core/geometry/DirectVector.h"
#include "core/rec_space/ReciprocalVector.h"

#include "core/detector/DetectorEvent.h"

#include "core/data/DataSet.h"
#include "core/integration/Profile3D.h"
#include "core/data/MetaData.h"
#include "core/data/ILLDataReader.h"
#include "core/data/IDataReader.h"
#include "core/data/HDF5DataReader.h"
#include "core/data/HDF5MetaDataReader.h"

#include "core/data/RawDataReader.h"
#include "core/data/TiffDataReader.h"
#include "core/data/BloscFilter.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/data/DataReaderFactory.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorFactory.h"
#include "core/axes/TransAxis.h"
#include "core/instrument/Sample.h"
#include "core/detector/FlatDetector.h"
#include "core/instrument/Source.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InterpolatedState.h"

#include "core/instrument/Monochromator.h"
#include "core/instrument/Diffractometer.h"
#include "core/detector/CylindricalDetector.h"
#include "core/axes/Gonio.h"

#include "core/search_peaks/Convolver.h"
#include "core/search_peaks/AtomicConvolver.h"
#include "core/search_peaks/DeltaConvolver.h"
#include "core/search_peaks/AnnularConvolver.h"
#include "core/search_peaks/EnhancedAnnularConvolver.h"
#include "core/search_peaks/ConstantConvolver.h"
#include "core/search_peaks/ConvolverFactory.h"
#include "core/search_peaks/RadialConvolver.h"

#include "core/monte-carlo/mosaic.h"

#include "core/axes/Axis.h"
#include "core/instrument/Experiment.h"

#include "core/instrument/Component.h"
#include "core/axes/AxisFactory.h"
#include "core/axes/RotAxis.h"

#include "core/utils/Singleton.h"
#include "core/utils/Factory.h"

#include "core/utils/Enums.h"

#include "core/axes/Axis.h"
#include "core/axes/RotAxis.h"
#include "core/axes/TransAxis.h"

#include "core/instrument/Monochromator.h"

#include "core/detector/Detector.h"
#include "core/detector/CylindricalDetector.h"
#include "core/detector/FlatDetector.h"

#include "core/instrument/Source.h"

#include "core/instrument/Sample.h"

#include "core/geometry/AABB.h"
#include "core/geometry/Ellipsoid.h"
#include "core/geometry/Blob3D.h"

#include "core/auto_indexing/FFTIndexing.h"
#include "core/crystal/Peak3D.h"
#include "core/crystal/SpaceGroup.h"
#include "core/crystal/UnitCell.h"
#include "core/crystal/GruberReduction.h"

#include "core/crystal/Intensity.h"

using sptrUnitCell = std::shared_ptr<nsx::UnitCell>;

#include "core/instrument/Diffractometer.h"

using sptrDiffractometer = std::shared_ptr<nsx::Diffractometer>;

#include "core/utils/Singleton.h"

#include "core/search_peaks/Convolver.h"
#include "core/search_peaks/AtomicConvolver.h"
#include "core/search_peaks/ConvolverFactory.h"
#include "core/search_peaks/DeltaConvolver.h"
#include "core/search_peaks/AnnularConvolver.h"
#include "core/search_peaks/EnhancedAnnularConvolver.h"
#include "core/search_peaks/ConstantConvolver.h"
#include "core/search_peaks/RadialConvolver.h"

#include "core/data/MetaData.h"
#include "core/data/IDataReader.h"
#include "core/data/DataReaderFactory.h"
#include "core/data/ILLDataReader.h"
#include "core/data/HDF5DataReader.h"
#include "core/data/DataSet.h"
#include "core/search_peaks/PeakFinder.h"
#include "core/data/MergedData.h"

#include "core/crystal/PeakFilter.h"

#include "core/crystal/MillerIndex.h"
#include "core/integration/Profile1D.h"
#include "core/integration/ShapeLibrary.h"
using sptrShapeLibrary = std::shared_ptr<nsx::ShapeLibrary>;

#include "core/statistics/CC.h"
#include "core/statistics/RFactor.h"

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

%include "core/chemistry/ChemistryTypes.h"
%include "core/crystal/CrystalTypes.h"
%include "core/data/DataTypes.h"
%include "core/geometry/GeometryTypes.h"
%include "core/instrument/InstrumentTypes.h"
%include "core/mathematics/MathematicsTypes.h"
%include "core/utils/UtilsTypes.h"

%template(UnitCellList) std::vector<nsx::sptrUnitCell>;

%include "core/geometry/DirectVector.h"
%template(DirectVectorList) std::vector<nsx::DirectVector>;

%include "core/rec_space/ReciprocalVector.h"
%template(ReciprocalVectorList) std::vector<nsx::ReciprocalVector>;
%template(ReciprocalVectorQueue) std::deque<nsx::ReciprocalVector>;

%include "core/axes/Axis.h"
%include "core/axes/RotAxis.h"
%include "core/axes/TransAxis.h"
%include "core/axes/Gonio.h"
%include "core/instrument/Component.h"
%include "core/instrument/Monochromator.h"
%include "core/instrument/Source.h"

%include "core/geometry/AABB.h"
%include "core/geometry/Ellipsoid.h"
%include "core/geometry/Blob3D.h"

%include "core/utils/Any.h"
%extend nsx::Any {
     %template(Anyi) Any<int>;
     %template(Anyd) Any<double>;
     %template(Anyb) Any<bool>;
     %template(Anys) Any<std::string>;
}

%include "core/chemistry/Material.h"
%include "core/chemistry/IsotopeDatabaseManager.h"
%template(propertyi) nsx::IsotopeDatabaseManager::property<int>;
%template(propertyd) nsx::IsotopeDatabaseManager::property<double>;
%template(propertys) nsx::IsotopeDatabaseManager::property<std::string>;
%template(propertyb) nsx::IsotopeDatabaseManager::property<bool>;

%template(scored_uc) std::pair<std::shared_ptr<nsx::UnitCell>, double>;
%template(indexer_solutions) std::vector<std::pair<std::shared_ptr<nsx::UnitCell>,double>>;

%include "core/instrument/InstrumentTypes.h"
%include "core/detector/Detector.h"
%include "core/detector/DetectorEvent.h"
%template(DetectorEventQueue) std::deque<nsx::DetectorEvent>;
%template(DetectorEventList) std::vector<nsx::DetectorEvent>;
%template(DoubleQueue) std::deque<double>;
%include "core/detector/CylindricalDetector.h"
%include "core/detector/FlatDetector.h"
%include "core/instrument/Diffractometer.h"
%include "core/instrument/Sample.h"
%include "core/utils/Singleton.h"
%include "core/instrument/InstrumentState.h"
%include "core/instrument/InterpolatedState.h"

namespace nsx {
   class DataReaderFactory; 
   class ConvolverFactory; 
   struct tVector;
}

%include "core/auto_indexing/FFTIndexing.h"


%include "core/crystal/PeakData.h"
%include "core/integration/IntegrationRegion.h"
%include "core/crystal/Intensity.h"
%template(IntensityList) std::vector<nsx::Intensity>;
%include "core/integration/Profile3D.h"
%include "core/integration/Profile1D.h"
%include "core/integration/IPeakIntegrator.h"
%include "core/integration/MeanBackgroundIntegrator.h"
%include "core/integration/Profile3DIntegrator.h"
%include "core/integration/Profile1DIntegrator.h"
%include "core/integration/ISigmaIntegrator.h"
%include "core/integration/PixelSumIntegrator.h"
%include "core/crystal/Peak3D.h"
%include "core/integration/Profile1D.h"
%include "core/integration/ShapeLibrary.h"
%include "core/integration/ShapeIntegrator.h"
%include "core/integration/IntegrationRegion.h"
%include "core/crystal/PeakData.h"
%include "core/crystal/Intensity.h"
%include "core/crystal/Peak3D.h"

%template(PeakList) std::vector<std::shared_ptr<nsx::Peak3D>>;
%template(PeakShell) std::vector<std::vector<std::shared_ptr<nsx::Peak3D>>>;

%include "core/crystal/PeakFilter.h"

%template(ConvolverParameters) std::map<std::string,double>;
%include "core/search_peaks/Convolver.h"
%include "core/search_peaks/ConvolverFactory.h"
%include "core/search_peaks/ConstantConvolver.h"
%include "core/search_peaks/DeltaConvolver.h"
%include "core/search_peaks/AnnularConvolver.h"
%include "core/search_peaks/EnhancedAnnularConvolver.h"
%include "core/search_peaks/RadialConvolver.h"
%include "core/search_peaks/Convolver.h"
%include "core/search_peaks/AtomicConvolver.h"

%include "core/monte-carlo/mosaic.h"

%include "core/data/MetaData.h"
%include "core/data/IDataReader.h"
%include "core/data/DataReaderFactory.h"
%include "core/data/ILLDataReader.h"
%include "core/data/HDF5MetaDataReader.h"
%include "core/data/HDF5DataReader.h"
%include "core/integration/Profile3D.h"
%include "core/data/DataSet.h"


%include "core/data/MergedData.h"

%template(DataList) std::vector<std::shared_ptr<nsx::DataSet>>;

%include "core/search_peaks/PeakFinder.h"

%template(MergedPeakSet) std::set<nsx::MergedPeak>;

%include "core/fitting/FitParameters.h"
%include "core/fitting/Minimizer.h"

%include "core/data/EigenToVector.h"
%include "core/utils/System.h"
%include "core/utils/CSV.h"
%include "core/utils/Enums.h"
%include "core/utils/DoubleToFraction.h"
%include "core/utils/Path.h"

%include "core/data/Parser.h"
%include "core/data/MatrixParser.h"
%include "core/utils/ProgressHandler.h"

%include "core/crystal/PeakCoordinateSystem.h"
%include "core/refine/RefinementBatch.h"
%include "core/refine/Refiner.h"

%template(RefinementBatchList) std::vector<nsx::RefinementBatch>;

%include "core/crystal/Intensity.h"

%include "core/integration/IntegrationRegion.h"
%include "core/crystal/PeakData.h"


%include "core/crystal/UnitCell.h"
%include "core/crystal/ResolutionShell.h"

%include "core/statistics/RFactor.h"
%include "core/statistics/CC.h"
%include "core/statistics/CC.h"

%include "core/crystal/Peak3D.h"

%include "core/crystal/MillerIndex.h"
%template(MillerIndexList) std::vector<nsx::MillerIndex>;

%include "core/integration/Profile1D.h"
%include "core/integration/ShapeLibrary.h"

%include "core/auto_indexing/FFTIndexing.h"
%include "core/crystal/MergedPeak.h"
%include "core/crystal/SpaceGroup.h"
%include "core/crystal/NiggliReduction.h"
%include "core/crystal/GruberReduction.h"
%include "core/crystal/SymOp.h"
%template(SymOpList) std::vector<nsx::SymOp>;

%include "core/geometry/Vertex.h"
%include "core/geometry/Triangle.h"
%include "core/geometry/Ellipsoid.h"
%include "core/geometry/Face.h"
%include "core/monte-carlo/MCAbsorption.h"
%include "core/geometry/ConvexHull.h"
%include "core/geometry/AABB.h"
%include "core/geometry/Edge.h"
%include "core/geometry/Blob3D.h"

%include "core/crystal/BrillouinZone.h"

%include "core/integration/Profile3D.h"
%include "core/data/DataSet.h"
%include "core/data/MetaData.h"
%include "core/data/ILLDataReader.h"
%include "core/data/IDataReader.h"
%include "core/data/HDF5MetaDataReader.h"
%include "core/data/HDF5DataReader.h"

%include "core/data/RawDataReader.h"
%include "core/data/TiffDataReader.h"
%include "core/data/BloscFilter.h"
%include "core/data/DataReaderFactory.h"

%include "core/search_peaks/Convolver.h"
%include "core/search_peaks/AtomicConvolver.h"
%include "core/search_peaks/ConstantConvolver.h"
%include "core/search_peaks/DeltaConvolver.h"
%include "core/search_peaks/AnnularConvolver.h"
%include "core/search_peaks/EnhancedAnnularConvolver.h"
%include "core/search_peaks/RadialConvolver.h"
%include "core/search_peaks/ConvolverFactory.h"

%include "core/detector/Detector.h"
%include "core/detector/DetectorFactory.h"
%include "core/axes/TransAxis.h"
%include "core/instrument/Sample.h"
%include "core/detector/FlatDetector.h"
%include "core/instrument/Source.h"
%include "core/instrument/InstrumentState.h"
%include "core/instrument/InterpolatedState.h"

%include "core/instrument/Monochromator.h"
%include "core/instrument/Diffractometer.h"
%include "core/detector/CylindricalDetector.h"


%template(InstrumentStateList) std::vector<nsx::InstrumentState>;


%include "core/axes/Axis.h"
%include "core/instrument/Experiment.h"
%include "core/axes/AxisFactory.h"
%include "core/axes/RotAxis.h"

%include "core/auto_indexing/AutoIndexer.h"

%include "core/utils/Singleton.h"

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
