%module(directors="1") "pynsx"

%include "cpointer.i"
%include "exception.i"
%include "typemaps.i"
%include "warnings.i"

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

%include <std_deque.i>
%include <std_map.i>
%include <std_pair.i>
%include <std_set.i>
%include <std_shared_ptr.i>
%include <std_string.i>
%include <std_vector.i>
%include <typemaps.i>

%template(vector_1d)  std::vector<double>;
%template(vector_2d)  std::vector<std::vector<double>>;
%template(vector_1i) std::vector<int>;
%template(vector_2i) std::vector<std::vector<int>>;
%template(vector_string) std::vector<std::string>;

%shared_ptr(nsx::DataSet)
%shared_ptr(nsx::Diffractometer)
%shared_ptr(nsx::Experiment)
%shared_ptr(nsx::HDF5DataReader)
%shared_ptr(nsx::HDF5MetaDataReader)
%shared_ptr(nsx::IDataReader)
%shared_ptr(nsx::ILLDataReader)
%shared_ptr(nsx::Peak3D)
%shared_ptr(nsx::ProgressHandler)
%shared_ptr(nsx::RawDataReader)
%shared_ptr(nsx::ShapeLibrary)
%shared_ptr(nsx::TiffDataReader)
%shared_ptr(nsx::UnitCell)

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

#include "base/fit/FitParameters.h"
#include "base/fit/Minimizer.h"
#include "base/geometry/AABB.h"
#include "base/geometry/DirectVector.h"
#include "base/geometry/Ellipsoid.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/hull/ConvexHull.h"
#include "base/hull/Edge.h"
#include "base/hull/Face.h"
#include "base/hull/Triangle.h"
#include "base/hull/Vertex.h"
#include "base/parser/BloscFilter.h"
#include "base/parser/EigenToVector.h"
#include "base/parser/Parser.h"
#include "base/utils/CSV.h"
#include "base/utils/Factory.h"
#include "base/utils/Path.h"
#include "base/utils/ProgressHandler.h"
#include "base/utils/Singleton.h"
#include "base/utils/Units.h"

#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/NiggliReduction.h"
#include "tables/crystal/SymOp.h"
#include "tables/crystal/SpaceGroup.h"
#include "tables/crystal/UnitCell.h"
#include "tables/crystal/BrillouinZone.h"
#include "tables/crystal/GruberReduction.h"

#include "core/algo/AutoIndexer.h"
#include "core/algo/DataReaderFactory.h"
#include "core/algo/FFTIndexing.h"
#include "core/algo/RefinementBatch.h"
#include "core/algo/Refiner.h"
#include "core/analyse/Blob3D.h"
#include "core/analyse/MergedPeak.h"
#include "core/analyse/PeakFilter.h"
#include "core/analyse/PeakFinder.h"
#include "core/convolve/AnnularConvolver.h"
#include "core/convolve/AtomicConvolver.h"
#include "core/convolve/ConstantConvolver.h"
#include "core/convolve/Convolver.h"
#include "core/convolve/ConvolverFactory.h"
#include "core/convolve/DeltaConvolver.h"
#include "core/convolve/EnhancedAnnularConvolver.h"
#include "core/convolve/RadialConvolver.h"
#include "core/detector/CylindricalDetector.h"
#include "core/detector/DataOrder.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/detector/DetectorFactory.h"
#include "core/detector/FlatDetector.h"
#include "core/detector/MatrixParser.h"
#include "core/experiment/DataSet.h"
#include "core/experiment/Experiment.h"
#include "core/gonio/Axis.h"
#include "core/gonio/AxisFactory.h"
#include "core/gonio/Component.h"
#include "core/gonio/Gonio.h"
#include "core/gonio/RotAxis.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InterpolatedState.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/integration/ISigmaIntegrator.h"
#include "core/integration/MeanBackgroundIntegrator.h"
#include "core/integration/PixelSumIntegrator.h"
#include "core/integration/Profile1DIntegrator.h"
#include "core/integration/Profile3DIntegrator.h"
#include "core/integration/ShapeIntegrator.h"
#include "core/loader/HDF5DataReader.h"
#include "core/loader/HDF5MetaDataReader.h"
#include "core/loader/ILLDataReader.h"
#include "core/loader/RawDataReader.h"
#include "core/loader/TiffDataReader.h"
#include "core/monte-carlo/MCAbsorption.h"
#include "core/peak/IPeakIntegrator.h"
#include "core/peak/IntegrationRegion.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/peak/PeakCoordinateSystem.h"
#include "core/peak/PeakData.h"
#include "core/raw/IDataReader.h"
#include "core/raw/MetaData.h"
#include "core/shape/Profile1D.h"
#include "core/shape/Profile3D.h"
#include "core/shape/ShapeLibrary.h"
#include "core/statistics/CC.h"
#include "core/statistics/RFactor.h"
#include "core/statistics/ResolutionShell.h"

#include "core/output/PeakExporter.h"

using namespace nsx;

%}

%include "numpy.i"
%include "eigen.i"

%init %{
    import_array();
%}

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

// Include hierarchy from bottom to top:
//   no %-included file must depend on files that have not been %-included before.

%include "base/utils/CSV.h"
%include "base/utils/Path.h"
%include "base/utils/ProgressHandler.h"

%include "base/geometry/DirectVector.h"
%template(DirectVectorList) std::vector<nsx::DirectVector>;

%include "base/geometry/ReciprocalVector.h"
%template(ReciprocalVectorList) std::vector<nsx::ReciprocalVector>;
%template(ReciprocalVectorQueue) std::deque<nsx::ReciprocalVector>;

%include "base/geometry/AABB.h"
%include "base/geometry/Ellipsoid.h"

%include "base/hull/ConvexHull.h"
%include "base/hull/Edge.h"
%include "base/hull/Vertex.h"
%include "base/hull/Triangle.h"
%include "base/hull/Face.h"

%include "base/parser/BloscFilter.h"
%include "base/parser/EigenToVector.h"
%include "base/parser/Parser.h"

%include "base/fit/FitParameters.h"
%include "base/fit/Minimizer.h"

%include "tables/crystal/SymOp.h"
%template(SymOpList) std::vector<nsx::SymOp>;
%include "tables/crystal/MillerIndex.h"
%template(MillerIndexList) std::vector<nsx::MillerIndex>;
%include "tables/crystal/SpaceGroup.h"
%include "tables/crystal/NiggliReduction.h"
%include "tables/crystal/UnitCell.h"
%template(UnitCellList) std::vector<nsx::sptrUnitCell>;
%template(indexer_solutions) std::vector<std::pair<std::shared_ptr<nsx::UnitCell>,double>>;
%include "tables/crystal/GruberReduction.h"
%include "tables/crystal/BrillouinZone.h"

%include "tables/crystal/UnitCell.h"
%include "tables/crystal/SpaceGroup.h"

%include "core/gonio/Axis.h"
%include "core/gonio/Gonio.h"
%include "core/gonio/Component.h"
%include "core/gonio/AxisFactory.h"
%include "core/gonio/RotAxis.h"

%include "core/detector/Detector.h"
%include "core/detector/CylindricalDetector.h"
%include "core/detector/FlatDetector.h"
%include "core/detector/DataOrder.h"
%include "core/detector/MatrixParser.h"
%include "core/detector/DetectorFactory.h"
%include "core/detector/DetectorEvent.h"
%template(DetectorEventQueue) std::deque<nsx::DetectorEvent>;
%template(DetectorEventList) std::vector<nsx::DetectorEvent>;

%include "core/instrument/InstrumentTypes.h"
%include "core/instrument/Monochromator.h"
%include "core/instrument/Diffractometer.h"
%include "core/instrument/Sample.h"
%include "core/instrument/InstrumentState.h"
%include "core/instrument/InterpolatedState.h"
%include "core/instrument/InstrumentStateList.h"
%include "core/instrument/Source.h"

%include "core/raw/MetaData.h"
%include "core/raw/IDataReader.h"

%include "core/loader/ILLDataReader.h"
%include "core/loader/HDF5MetaDataReader.h"
%include "core/loader/HDF5DataReader.h"
%include "core/loader/RawDataReader.h"
%include "core/loader/TiffDataReader.h"

%include "core/peak/Intensity.h"
%include "core/peak/Peak3D.h"
%include "core/peak/PeakCoordinateSystem.h"
%include "core/peak/PeakData.h"
%include "core/peak/IntegrationRegion.h"
%template(PeakList) std::vector<std::shared_ptr<nsx::Peak3D>>;

%include "core/experiment/DataSet.h"
%include "core/experiment/DataTypes.h"
%template(DataList) std::vector<std::shared_ptr<nsx::DataSet>>;
%include "core/experiment/Experiment.h"

%include "core/peak/IPeakIntegrator.h"

%template(ConvolverParameters) std::map<std::string,double>;
%include "core/convolve/Convolver.h"
%include "core/convolve/ConvolverFactory.h"
%include "core/convolve/AnnularConvolver.h"
%include "core/convolve/AtomicConvolver.h"
%include "core/convolve/ConstantConvolver.h"
%include "core/convolve/DeltaConvolver.h"
%include "core/convolve/EnhancedAnnularConvolver.h"
%include "core/convolve/RadialConvolver.h"

%include "core/shape/Profile3D.h"
%include "core/shape/Profile1D.h"
%include "core/shape/ShapeLibrary.h"

%include "core/analyse/MergedPeak.h"
%include "core/analyse/Blob3D.h"
%include "core/analyse/PeakFilter.h"
%include "core/analyse/Blob3D.h"
%include "core/analyse/MergedData.h"
%include "core/analyse/PeakFinder.h"
%template(MergedPeakSet) std::set<nsx::MergedPeak>;

%include "core/algo/DataReaderFactory.h"
%include "core/algo/RefinementBatch.h"
%include "core/algo/Refiner.h"
%include "core/algo/FFTIndexing.h"
%include "core/algo/AutoIndexer.h"

%include "core/integration/MeanBackgroundIntegrator.h"
%include "core/integration/Profile3DIntegrator.h"
%include "core/integration/Profile1DIntegrator.h"
%include "core/integration/ISigmaIntegrator.h"
%include "core/integration/PixelSumIntegrator.h"
%include "core/integration/ShapeIntegrator.h"

%template(RefinementBatchList) std::vector<nsx::RefinementBatch>;

%include "core/monte-carlo/MCAbsorption.h"

%include "core/statistics/ResolutionShell.h"
%include "core/statistics/RFactor.h"
%include "core/statistics/CC.h"
%include "core/statistics/CC.h"

%include "core/output/PeakExporter.h"

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
