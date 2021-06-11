//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/ExperimentExporter.cpp
//! @brief     Implements class ExperimentExporter.
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/ExperimentExporter.h"

#include "base/parser/BloscFilter.h"
#include "base/utils/Logger.h"
#include "base/utils/Units.h" // deg
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/peak/Peak3D.h"
#include "core/raw/MetaData.h" // MetaDataMap
#include "tables/crystal/UnitCell.h"
#include "core/instrument/Diffractometer.h"
#include "core/shape/PeakCollection.h"

#include <Eigen/Dense>
#include <iostream>
#include <vector>
#include <tuple>

#include "H5Cpp.h"

namespace {
// HDF5 DataTypes
static const H5::StrType str80Type(H5::PredType::C_S1, 80);
static const H5::StrType strVarType(H5::PredType::C_S1, H5T_VARIABLE);

// HDF5 DataSpace (defining data shape)
static const H5::DataSpace metaSpace(H5S_SCALAR);

// state vector
using statesVec = std::vector< std::vector<double> >;
const H5::DataType stateValueType {H5::PredType::NATIVE_DOUBLE};

// Peak metadata type
using PeakMeta = std::map<std::string, float>;

// write functions
inline
void writeAttribute(H5::H5File& file, const std::string& key, const void* const value,
                    const H5::DataType& datatype, const H5::DataSpace& dataspace)
{
    H5::Attribute attr(file.createAttribute(key, datatype, dataspace));
    attr.write(datatype, value);
}

void writeDetectorState(H5::H5File& file, const std::string& datakey,
                        const nsx::DataSet* const dataset)
{
    const std::string detectorKey = datakey + "/Detector";

    file.createGroup(detectorKey);

    const std::size_t n_frames = dataset->nFrames();
    const hsize_t nf[1] = {n_frames};
    const H5::DataSpace scanSpace(1, nf);
    Eigen::VectorXd values(n_frames);

    const statesVec& detectorStates = dataset->reader()->detectorStates();
    const nsx::Gonio& detector_gonio = dataset->reader()->diffractometer()->detector()->gonio();
    const std::size_t n_detector_gonio_axes = detector_gonio.nAxes();
    for (std::size_t i_axis = 0; i_axis < n_detector_gonio_axes; ++i_axis) {
        for (std::size_t i_frame = 0; i_frame < n_frames; ++i_frame)
            values(i_frame) = detectorStates[i_frame][i_axis] / nsx::deg;  // TODO: check the unit

        H5::DataSet detector_scan(
            file.createDataSet(std::string(detectorKey + "/" + detector_gonio.axis(i_axis).name()),
                               stateValueType, scanSpace));
        detector_scan.write(&values(0), stateValueType, scanSpace, scanSpace);
    }
}


void writeSampleState(H5::H5File& file, const std::string& datakey,
                      const nsx::DataSet* const dataset)
{
    const std::string sampleKey = datakey + "/Sample";
    const std::size_t n_frames = dataset->nFrames();
    const hsize_t nf[1] = {n_frames};
    Eigen::VectorXd values(n_frames);
    const H5::DataSpace scanSpace(1, nf);
    const statesVec& sampleStates = dataset->reader()->sampleStates();
    const nsx::Gonio& sample_gonio = dataset->reader()->diffractometer()->sample().gonio();
    std::size_t n_sample_gonio_axes = sample_gonio.nAxes();

    file.createGroup(sampleKey);
    for (std::size_t i_axis = 0; i_axis < n_sample_gonio_axes; ++i_axis) {
        const auto& axis = sample_gonio.axis(i_axis);
        for (std::size_t i_frame = 0; i_frame < n_frames; ++i_frame)
            values(i_frame) = sampleStates[i_frame][i_axis] / nsx::deg;  // TODO: check the unit
        H5::DataSet sample_scan(file.createDataSet(
                                                   std::string(sampleKey + "/" + axis.name()),
                                                   stateValueType, scanSpace));
        sample_scan.write(&values(0), stateValueType, scanSpace, scanSpace);
    }
}


void writeMetaInfo(H5::H5File& file, const std::string& datakey,
                   const nsx::DataSet* const dataset)
{
    // Write all string metadata into the "Info" group
    const std::string infoKey = datakey + "/Info";
    // Write all other metadata (int and double) into the "Meta" Group
    const std::string metaKey = datakey + "/Meta";  // TODO: Why different from Info?

    H5::Group info_group = file.createGroup(infoKey);
    H5::Group meta_group = file.createGroup(metaKey);

    try {
        for (const auto& [key, val] : dataset->metadata().map()) {
            if (std::holds_alternative<std::string>(val)) {
                const std::string& info = std::get<std::string>(val);
                H5::Attribute intAtt(info_group.createAttribute(key, str80Type, metaSpace));
                intAtt.write(str80Type, info);
            } else if (std::holds_alternative<int>(val)) {
                const int value = std::get<int>(val);
                H5::Attribute intAtt(meta_group.createAttribute
                                     (key, H5::PredType::NATIVE_INT32, metaSpace));
                intAtt.write(H5::PredType::NATIVE_INT, &value);
            } else if (std::holds_alternative<double>(val)) {
                const double d_value = std::get<double>(val);
                H5::Attribute doubleAtt(meta_group.createAttribute
                                        (key, H5::PredType::NATIVE_DOUBLE, metaSpace));
                doubleAtt.write(H5::PredType::NATIVE_DOUBLE, &d_value);
            }
        }
    } catch (const std::exception& ex) {
        nsxlog(nsx::Level::Debug, "Exception in", __PRETTY_FUNCTION__, ":", ex.what());
    }
}

// TODO: Merge with writeMetaInfo
// TODO: PeakCollection metadata is map<string, float> but used as map<string, int> !
// TODO: Unify the metadata structure for all objects
void writePeakMeta(H5::H5File& file, const std::string& datakey,
                   const PeakMeta& pmeta, const nsx::listtype type)
{
    const std::string metaKey = datakey + "/Meta";  // TODO: Why different from Info?

    H5::Group peak_meta_group = file.createGroup(metaKey);
    H5::DataSpace metaSpace(H5S_SCALAR);
    H5::StrType str80Type(H5::PredType::C_S1, 80);

    try {
        for (const auto& [key, val] : pmeta) {
            const int value = static_cast<int>(val);
            H5::Attribute intAtt(peak_meta_group.createAttribute
                                 (key, H5::PredType::NATIVE_INT32, metaSpace));
            intAtt.write(H5::PredType::NATIVE_INT, &value);
        }
    } catch (const std::exception& ex) {
        nsxlog(nsx::Level::Debug, "Exception in", __PRETTY_FUNCTION__, ":", ex.what());
    }

    const int listtype_int = static_cast<int>(type);
    H5::Attribute type_att
        (peak_meta_group.createAttribute("Type", H5::PredType::NATIVE_INT32, metaSpace));
    type_att.write(H5::PredType::NATIVE_INT32, &listtype_int);
}

void writePeakDataNames(H5::H5File& file, const std::string& datakey,
                        const std::vector<std::string> data_names)
{
    const std::size_t nPeaks = data_names.size();
    std::vector<const char*> data_name_pointers(nPeaks);
    for (std::size_t i = 0; i < nPeaks; ++i)
        data_name_pointers[i] = data_names[i].c_str();

    const hsize_t num_peaks[1] = {nPeaks};
    const H5::DataSpace peak_space(1, num_peaks);
    H5::DataSet data_H5
        (file.createDataSet(datakey, strVarType, peak_space));
    data_H5.write(data_name_pointers.data(), strVarType, peak_space, peak_space);
}


void writeFrames(H5::H5File& file, const std::map<std::string, nsx::DataSet*> data)
{
    //-- BLOSC configuration
    blosc_init();
    blosc_set_nthreads(4);
    { // Register BLOSC
        char *version, *date;
        const int register_status = register_blosc(&version, &date);
        if (register_status <= 0)
            throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

        /* NOTE:
           BLOSC register_status stores the version and the date with `strdup`
           *version = strdup(BLOSC_VERSION_STRING);
           *date = strdup(BLOSC_VERSION_DATE);

           Therefore version and date must be freed afterwards.
        */
        free(version);
        free(date);
    }

    // speed/compression for diffraction data
    unsigned int cd_values[7]; // 0 to 3 (inclusive) param slots are reserved.
    cd_values[4] = 9; // Highest compression level
    cd_values[5] = 1; // Bit shuffling active; 0: shuffle not active, 1: shuffle active
    cd_values[6] = BLOSC_BLOSCLZ; // Actual compressor to use: BLOSC seem to be the best compromise
    //-- END BLOSC configuration

    using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    const std::string dataCollectionsKey = "/DataCollections";
    file.createGroup(dataCollectionsKey);

    for (const auto& it : data) {
        const nsx::DataSet* data_item = it.second;
        const std::string name = data_item->name();
        const std::string datakey = dataCollectionsKey + "/" + name;
        const std::size_t n_frames = data_item->nFrames(),
            n_rows = data_item->nRows(), n_cols = data_item->nCols();

        const hsize_t chunk[3] = {1, n_rows, n_cols};  // chunk for Blosc // TODO: check this

        H5::DSetCreatPropList plist;
        plist.setChunk(3, chunk);
        plist.setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);

        H5::Group data_collection{file.createGroup(datakey)};

        // H5::DataSet for frames
        const hsize_t dims[3] = {n_frames, n_rows, n_cols};
        const H5::DataSpace space(3, dims, nullptr);
        const H5::DataType frameType {H5::PredType::NATIVE_INT32};
        H5::DataSet dset(file.createDataSet
                         (std::string(datakey + "/" + name), frameType, space, plist));

        // Write frames
        const hsize_t count[3] = {1, n_rows, n_cols};  // TODO: is this `dims` for a single frame?
        hsize_t offset[3] = {0, 0, 0};

        H5::DataSpace memspace(3, count, nullptr);
        for (offset[0] = 0; offset[0] < n_frames; offset[0] += count[0]) {
            // TODO: Explain the slab
            space.selectHyperslab(H5S_SELECT_SET, count, offset, nullptr, nullptr);
            // HDF5 requires row-major storage, so copy frame into a row-major matrix
            // TODO: check if this is really necessary
            IntMatrix current_frame(data_item->frame(offset[0]));
            dset.write(current_frame.data(), frameType, memspace, space);
        }
    }

    blosc_destroy();
}

} // namespace


namespace nsx {

void ExperimentExporter::createFile(std::string name, std::string diffractometer, std::string path)
{
    H5::H5File file{path.c_str(), H5F_ACC_TRUNC};
    _file_name = path;  // store the filename for later use

    writeAttribute(file, "name", name.data(), str80Type, metaSpace);
    writeAttribute(file, "diffractometer", diffractometer.data(), str80Type, metaSpace);
}

void ExperimentExporter::writeData(const std::map<std::string, DataSet*> data)
{
    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    writeFrames(file, data);

    const std::string dataCollectionsKey = "/DataCollections";
    for (const auto& it : data) {
        const DataSet* data_item = it.second;
        const std::string datakey = dataCollectionsKey + "/" + data_item->name();

        // Write detector states
        writeDetectorState(file, datakey, data_item);

        // Write sample states
        writeSampleState(file, datakey, data_item);

        // Write all string metadata into the "Info" group
        // Write all other metadata (int and double) into the "Meta" Group
        writeMetaInfo(file, datakey, data_item);
    }

}


void ExperimentExporter::writeInstrument(const Diffractometer*)
{
    // TODO ...
}

void ExperimentExporter::writePeaks(const std::map<std::string, PeakCollection*> peakCollections)
{
    std::string peakCollectionsKey = "/PeakCollections";
    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    file.createGroup(peakCollectionsKey);

    for (const auto& it : peakCollections) {
        // Write the data
        const std::string collection_name = it.first;
        PeakCollection* const collection_item = it.second;
        const std::string collectionNameKey = peakCollectionsKey + "/" + collection_name;

        file.createGroup(collectionNameKey);

        // initialize doubles
        const std::size_t nPeaks = collection_item->numberOfPeaks();
        //TODO: no peak_begin?
        std::vector<double> peak_end(nPeaks);
        std::vector<double> bkg_begin(nPeaks);
        std::vector<double> bkg_end(nPeaks);
        std::vector<double> scale(nPeaks);
        std::vector<double> transmission(nPeaks);
        std::vector<double> intensity(nPeaks);
        std::vector<double> sigma(nPeaks);
        std::vector<double> mean_bkg_val(nPeaks);
        std::vector<double> mean_bkg_sig(nPeaks);

        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> center(nPeaks, 3);
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> metric(
            3 * nPeaks, 3);

        center.setZero(nPeaks, 3);
        metric.setZero(3 * nPeaks, 3);

        // initialize integers
        std::vector<int> rejection_flag(nPeaks);

        // initialize the booleans (NOTE: int is used instead of bool)
        std::vector<int> selected(nPeaks);
        std::vector<int> masked(nPeaks);
        std::vector<int> predicted(nPeaks);

        // initialize the datanames
        std::vector<std::string> data_names;
        std::vector<std::string> unit_cells;

        std::string name;
        std::string ext;
        std::string unit_cell_name;
        const std::string temp_name = "NONE";

        for (std::size_t i = 0; i < nPeaks; ++i) {
            nsx::Peak3D* const peak = collection_item->getPeak(i);

            // set the values
            peak_end[i] = peak->peakEnd();
            bkg_begin[i] = peak->bkgBegin();
            bkg_end[i] = peak->bkgEnd();
            scale[i] = peak->scale();
            transmission[i] = peak->transmission();
            intensity[i] = peak->rawIntensity().value();
            sigma[i] = peak->rawIntensity().sigma();
            mean_bkg_val[i] = peak->meanBackground().value();
            mean_bkg_sig[i] = peak->meanBackground().sigma();

            rejection_flag[i] = static_cast<int>(peak->rejectionFlag());

            selected[i] = static_cast<int>(peak->selected());
            masked[i] = static_cast<int>(peak->masked());
            predicted[i] = static_cast<int>(peak->predicted());

            data_names.push_back(peak->dataSet()->name());

            const UnitCell* unit_cell_ptr = peak->unitCell();
            unit_cell_name = unit_cell_ptr? unit_cell_ptr->name() : temp_name;
            unit_cells.push_back(unit_cell_name);

            Eigen::Vector3d temp_col = peak->shape().center();
            center.block(i, 0, 1, 3) = Eigen::RowVector3d{temp_col(0), temp_col(1), temp_col(2)};
            metric.block(i * 3, 0, 3, 3) = peak->shape().metric();
        }

        // TODO: explain! check size 2 vs 3!
        const hsize_t num_peaks[1] = {nPeaks};
        const H5::DataSpace peak_space(1, num_peaks);
        const hsize_t center_peaks_h[2] = {nPeaks, 3};
        const H5::DataSpace center_space(2, center_peaks_h);
        const hsize_t metric_peaks_h[2] = {nPeaks * 3, 3};
        const H5::DataSpace metric_space(2, metric_peaks_h);

        const std::vector< std::tuple<std::string, H5::DataType, H5::DataSpace, const void*> > peakData_defs
            {
             // NATIVE_DOUBLE
             {"PeakEnd", H5::PredType::NATIVE_DOUBLE, peak_space, peak_end.data()},
             {"BkgBegin", H5::PredType::NATIVE_DOUBLE, peak_space, bkg_begin.data()},
             {"BkgEnd", H5::PredType::NATIVE_DOUBLE, peak_space, bkg_end.data()},
             {"Scale", H5::PredType::NATIVE_DOUBLE, peak_space, scale.data()},
             {"Transmission", H5::PredType::NATIVE_DOUBLE, peak_space, transmission.data()},
             {"Intensity", H5::PredType::NATIVE_DOUBLE, peak_space, intensity.data()},
             {"Sigma", H5::PredType::NATIVE_DOUBLE, peak_space, sigma.data()},
             {"BkgIntensity", H5::PredType::NATIVE_DOUBLE, peak_space, mean_bkg_val.data()},
             {"BkgSigma", H5::PredType::NATIVE_DOUBLE, peak_space, mean_bkg_sig.data()},
             {"Center", H5::PredType::NATIVE_DOUBLE, center_space, center.data()},
             {"Metric", H5::PredType::NATIVE_DOUBLE, metric_space, metric.data()},
             // NATIVE_INT32
             {"Rejection", H5::PredType::NATIVE_INT32, peak_space, rejection_flag.data()},
             // NATIVE_HBOOL
             {"Selected", H5::PredType::NATIVE_HBOOL, peak_space, selected.data()},
             {"Masked", H5::PredType::NATIVE_HBOOL, peak_space, masked.data()},
             {"Predicted", H5::PredType::NATIVE_HBOOL, peak_space, predicted.data()}
            };

        for (const auto& [dkey, dtype, dspace, dptr] : peakData_defs) {
            H5::DataSet data_H5(
                        file.createDataSet(collectionNameKey + "/" + dkey, dtype, dspace));
            data_H5.write(dptr, dtype, dspace, dspace);
        }

        // Write DataNames
        writePeakDataNames(file, collectionNameKey + "/DataNames", data_names);
        // Write unit-cell names
        writePeakDataNames(file, collectionNameKey + "/UnitCells", unit_cells);

        // Write all other metadata (int and double) into the "Meta" Group
        const PeakMeta& pmeta = *(collection_item->meta());
        writePeakMeta(file, collectionNameKey, pmeta, collection_item->type());
    }
}

void ExperimentExporter::writeUnitCells(const std::map<std::string, UnitCell*> unit_cells)
{
    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    file.createGroup("/UnitCells");
    H5::DataSpace metaSpace(H5S_SCALAR);
    H5::StrType str80Type(H5::PredType::C_S1, 80);

    for (const auto& it : unit_cells) {
        // Write the data
        const std::string unit_cell_name = it.first;
        const UnitCell* unit_cell = it.second;

        const uint z_val = unit_cell->z();
        const double tolerance = unit_cell->indexingTolerance();
        const Eigen::MatrixX3d rec = unit_cell->reciprocalBasis();

        H5::Group unit_cell_group = file.createGroup(std::string("/UnitCells/" + unit_cell_name));

        H5::Attribute rec_00(
            unit_cell_group.createAttribute("rec_00", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute rec_01(
            unit_cell_group.createAttribute("rec_01", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute rec_02(
            unit_cell_group.createAttribute("rec_02", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute rec_10(
            unit_cell_group.createAttribute("rec_10", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute rec_11(
            unit_cell_group.createAttribute("rec_11", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute rec_12(
            unit_cell_group.createAttribute("rec_12", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute rec_20(
            unit_cell_group.createAttribute("rec_20", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute rec_21(
            unit_cell_group.createAttribute("rec_21", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute rec_22(
            unit_cell_group.createAttribute("rec_22", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute index_tolerance(unit_cell_group.createAttribute(
            "indexing_tolerance", H5::PredType::NATIVE_DOUBLE, metaSpace));
        H5::Attribute bravais(unit_cell_group.createAttribute("bravais", str80Type, metaSpace));
        H5::Attribute space_group(unit_cell_group.createAttribute("space_group", str80Type, metaSpace));
        H5::Attribute z(unit_cell_group.createAttribute("z", H5::PredType::NATIVE_UINT, metaSpace));

        rec_00.write(H5::PredType::NATIVE_DOUBLE, &rec(0, 0));
        rec_01.write(H5::PredType::NATIVE_DOUBLE, &rec(0, 1));
        rec_02.write(H5::PredType::NATIVE_DOUBLE, &rec(0, 2));
        rec_10.write(H5::PredType::NATIVE_DOUBLE, &rec(1, 0));
        rec_11.write(H5::PredType::NATIVE_DOUBLE, &rec(1, 1));
        rec_12.write(H5::PredType::NATIVE_DOUBLE, &rec(1, 2));
        rec_20.write(H5::PredType::NATIVE_DOUBLE, &rec(2, 0));
        rec_21.write(H5::PredType::NATIVE_DOUBLE, &rec(2, 1));
        rec_22.write(H5::PredType::NATIVE_DOUBLE, &rec(2, 2));

        index_tolerance.write(H5::PredType::NATIVE_DOUBLE, &tolerance);
        bravais.write(str80Type, unit_cell->bravaisTypeSymbol());
        space_group.write(str80Type, unit_cell->spaceGroup().symbol());
        z.write(H5::PredType::NATIVE_UINT, &(z_val));
    }
}

void ExperimentExporter::finishWrite()
{
    // _file.close();
}

} // namespace nsx
