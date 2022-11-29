//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/experiment/ExperimentExporter.cpp
//! @brief     Implements class ExperimentExporter.
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/ExperimentExporter.h"


#include "base/geometry/AABB.h"
#include "base/mask/BoxMask.h"
#include "base/mask/EllipseMask.h"
#include "base/mask/IMask.h"
#include "base/utils/Logger.h"
#include "base/utils/Units.h" // deg
#include "core/data/DataSet.h"
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/InstrumentState.h"
#include "core/instrument/InstrumentStateSet.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"
#include "core/raw/HDF5BloscFilter.h"
#include "core/raw/HDF5TableIO.h" // HDF5TableWriter
#include "core/raw/MetaData.h"
#include "core/shape/PeakCollection.h"
#include "manifest.h"
#include "tables/crystal/UnitCell.h"

#include <Eigen/Dense>
#include <H5PredType.h>
#include <sstream>
#include <string>
#include <tuple>

#include <H5Cpp.h>

// TODO: Add more log messages

namespace {

// state vector
using statesVec = std::vector<std::vector<double>>;

// Peak metadata type
using PeakMeta = std::map<std::string, float>;

// write functions
inline void writeAttribute(
    H5::Group& group, const std::string& key, const void* const value_ptr,
    const H5::DataType& datatype, const H5::DataSpace& dataspace)
{
    H5::Attribute attr(group.createAttribute(key, datatype, dataspace));
    attr.write(datatype, value_ptr);
}

void writeDetectorState(
    H5::H5File& file, const std::string& datakey, const ohkl::DataSet* const dataset)
{
    const std::string detectorKey = datakey + "/" + ohkl::gr_Detector;

    file.createGroup(detectorKey);

    const std::size_t n_frames = dataset->nFrames();
    const hsize_t nf[1] = {n_frames};
    const H5::DataSpace scanSpace(1, nf);
    const H5::DataType stateValueType{H5::PredType::NATIVE_DOUBLE};
    Eigen::VectorXd values(n_frames);

    const statesVec& detectorStates = dataset->diffractometer()->detectorStates;
    const ohkl::Gonio& detector_gonio = dataset->diffractometer()->detector()->gonio();
    const std::size_t n_detector_gonio_axes = detector_gonio.nAxes();
    for (std::size_t i_axis = 0; i_axis < n_detector_gonio_axes; ++i_axis) {
        for (std::size_t i_frame = 0; i_frame < n_frames; ++i_frame)
            values(i_frame) = detectorStates[i_frame][i_axis] / ohkl::deg; // TODO: check the unit

        H5::DataSet detector_scan(file.createDataSet(
            std::string(detectorKey + "/" + detector_gonio.axis(i_axis).name()), stateValueType,
            scanSpace));
        detector_scan.write(&values(0), stateValueType, scanSpace, scanSpace);
    }
}

void writeSampleState(
    H5::H5File& file, const std::string& datakey, const ohkl::DataSet* const dataset)
{
    const std::string sampleKey = datakey + "/" + ohkl::gr_Sample;
    const std::size_t n_frames = dataset->nFrames();
    const hsize_t nf[1] = {n_frames};
    Eigen::VectorXd values(n_frames);
    const H5::DataSpace scanSpace(1, nf);
    const H5::DataType stateValueType{H5::PredType::NATIVE_DOUBLE};
    const statesVec& sampleStates = dataset->diffractometer()->sampleStates;
    const ohkl::Gonio& sample_gonio = dataset->diffractometer()->sample().gonio();
    std::size_t n_sample_gonio_axes = sample_gonio.nAxes();

    file.createGroup(sampleKey);
    for (std::size_t i_axis = 0; i_axis < n_sample_gonio_axes; ++i_axis) {
        const auto& axis = sample_gonio.axis(i_axis);
        for (std::size_t i_frame = 0; i_frame < n_frames; ++i_frame)
            values(i_frame) = sampleStates[i_frame][i_axis] / ohkl::deg; // TODO: check the unit
        H5::DataSet sample_scan(file.createDataSet(
            std::string(sampleKey + "/" + axis.name()), stateValueType, scanSpace));
        sample_scan.write(&values(0), stateValueType, scanSpace, scanSpace);
    }
}

void writeMasks(H5::H5File& file, const std::string& datakey, const ohkl::DataSet* const dataset)
{
    const auto& masks = dataset->masks();
    std::size_t n_masks = masks.size();

    std::vector<int> maskTypes;
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> upper(n_masks, 3);
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> lower(n_masks, 3);

    const hsize_t nmask[1] = {n_masks};
    const hsize_t bb_h[2] = {n_masks, 3};
    const H5::DataSpace mask_space(1, nmask);
    const H5::DataSpace bb_lower_space{2, bb_h};
    const H5::DataSpace bb_upper_space{2, bb_h};

    Eigen::Vector3d temp_col;
    std::size_t count = 0;
    for (auto* mask : masks) {
        if (dynamic_cast<const ohkl::BoxMask*>(mask) != nullptr) {
            maskTypes.push_back(static_cast<int>(ohkl::MaskType::Rectangle));
        } else if (dynamic_cast<const ohkl::EllipseMask*>(mask) != nullptr) {
            maskTypes.push_back(static_cast<int>(ohkl::MaskType::Ellipse));
        }
        temp_col = mask->aabb().upper();
        upper.block(count, 0, 1, 3) = Eigen::RowVector3d(temp_col(0), temp_col(1), temp_col(2));
        temp_col = mask->aabb().lower();
        lower.block(count, 0, 1, 3) = Eigen::RowVector3d(temp_col(0), temp_col(1), temp_col(2));
        ++count;
    }

    const std::string maskKey = datakey + "/" + ohkl::gr_Masks;
    const std::string upperKey = maskKey + "/" + ohkl::ds_upperBound;
    const std::string lowerKey = maskKey + "/" + ohkl::ds_lowerBound;
    const std::string typeKey = maskKey + "/" + ohkl::ds_maskType;
    file.createGroup(maskKey);

    H5::DataSet type_H5(file.createDataSet(typeKey, H5::PredType::NATIVE_UINT, mask_space));
    H5::DataSet upper_H5(file.createDataSet(upperKey, H5::PredType::NATIVE_DOUBLE, bb_upper_space));
    H5::DataSet lower_H5(file.createDataSet(lowerKey, H5::PredType::NATIVE_DOUBLE, bb_lower_space));
    type_H5.write(maskTypes.data(), H5::PredType::NATIVE_UINT, mask_space, mask_space);
    upper_H5.write(upper.data(), H5::PredType::NATIVE_DOUBLE, bb_upper_space, bb_upper_space);
    lower_H5.write(lower.data(), H5::PredType::NATIVE_DOUBLE, bb_lower_space, bb_lower_space);
}

void writeMetadata(H5::Group& meta_group, const ohkl::MetaData& metadata)
{
    const H5::DataSpace metaSpace(H5S_SCALAR);
    const H5::StrType strVarType(H5::PredType::C_S1, H5T_VARIABLE);


    try {
        for (const auto& [key, val] : metadata.map()) {
            if (std::holds_alternative<std::string>(val)) {
                H5::Attribute attr(meta_group.createAttribute(key, strVarType, metaSpace));
                attr.write(strVarType, std::get<std::string>(val));
            } else if (std::holds_alternative<int>(val))
                writeAttribute(
                    meta_group, key, &std::get<int>(val), H5::PredType::NATIVE_INT32, metaSpace);
            else if (std::holds_alternative<double>(val))
                writeAttribute(
                    meta_group, key, &std::get<double>(val), H5::PredType::NATIVE_DOUBLE,
                    metaSpace);
        }
    } catch (const std::exception& ex) {
        ohklLog(ohkl::Level::Debug, "Exception in writeMetaData: ", ex.what());
    }
}

void writePeakDataNames(
    H5::H5File& file, const std::string& datakey, const std::vector<std::string> data_names)
{
    const std::size_t nPeaks = data_names.size();
    std::vector<const char*> data_name_pointers(nPeaks);
    for (std::size_t i = 0; i < nPeaks; ++i)
        data_name_pointers[i] = data_names[i].c_str();

    const hsize_t num_peaks[1] = {nPeaks};
    const H5::DataSpace peak_space(1, num_peaks);
    const H5::StrType strVarType(H5::PredType::C_S1, H5T_VARIABLE);

    H5::DataSet data_H5(file.createDataSet(datakey, strVarType, peak_space));
    data_H5.write(data_name_pointers.data(), strVarType, peak_space, peak_space);
}

void writeFrames(
    H5::H5File& file, const std::string& dataCollectionsKey,
    const std::map<std::string, ohkl::DataSet*> data)
{
    ohkl::HDF5BloscFilter blosc_filter;

    using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
    file.createGroup(dataCollectionsKey);

    for (const auto& it : data) {
        const ohkl::DataSet* data_item = it.second;
        const std::string name = data_item->name();
        const std::string datakey = dataCollectionsKey + "/" + name;
        const std::size_t n_frames = data_item->nFrames(), n_rows = data_item->nRows(),
                          n_cols = data_item->nCols();

        H5::DSetCreatPropList plist;
        const hsize_t chunk_1frm[3] = {1, n_rows, n_cols}; // each chunk is a frame
        plist.setChunk(3, chunk_1frm);
        plist.setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, blosc_filter.cd_values);

        H5::Group data_collection{file.createGroup(datakey)};

        // H5::DataSet for frames
        const hsize_t dims[3] = {n_frames, n_rows, n_cols};
        const H5::DataSpace space(3, dims, nullptr);
        const H5::DataType frameType{H5::PredType::NATIVE_INT32};
        H5::DataSet dset(file.createDataSet(
            std::string(datakey + "/" + ohkl::ds_Dataset), frameType, space, plist));

        // Write frames
        const hsize_t count_1frm[3] = {1, n_rows, n_cols};
        hsize_t offset[3] = {0, 0, 0};
        hsize_t& i_frame = offset[0];

        H5::DataSpace memspace(3, count_1frm, nullptr);
        for (i_frame = 0; i_frame < n_frames; i_frame += count_1frm[0]) {
            // hyperslab corresponds to a single frame
            // H5S_SELECT_SET replaces the existing selection with the parameters from this call
            space.selectHyperslab(H5S_SELECT_SET, count_1frm, offset);

            // HDF5 requires row-major storage, so copy frame into a row-major matrix
            /* TODO: initializing the row-major RowMatrixXi will make a _copy_ of the result of
               calling `DataSet::frame` which returns a _col-major_ matrix `Eigen::MatrixXi`.
               This copy is very costly and should be avoided.
               Instead of `Eigen::MatrixXi` one should use `Eigen::RowMatrixXi`,
               or store the matrix with col-major layout also in the HDF5 file.
             */
            RowMatrixXi current_frame(data_item->frame(i_frame));
            dset.write(current_frame.data(), frameType, memspace, space);
        }
    }
}

} // namespace

namespace ohkl {

void ExperimentExporter::createFile(std::string name, std::string diffractometer, std::string path)
{
    try {
        H5::H5File file{path.c_str(), H5F_ACC_TRUNC};
        _file_name = path; // store the filename for later use

        const H5::DataSpace metaSpace(H5S_SCALAR);
        const H5::StrType str80Type(
            H5::PredType::C_S1, 80); // TODO: Make 80-chr restriction also in the GUI
        std::string hash = COMMIT_HASH;
        std::string version = VERSION;
        writeAttribute(file, ohkl::at_experiment, name.data(), str80Type, metaSpace);
        writeAttribute(file, ohkl::at_diffractometer, diffractometer.data(), str80Type, metaSpace);
        writeAttribute(file, ohkl::at_ohklVersion, version.data(), str80Type, metaSpace);
        writeAttribute(file, ohkl::at_commitHash, hash.data(), str80Type, metaSpace);
    } catch (...) {
        ohklLog(
            ohkl::Level::Error, "ExperimentExporter: Failed to create the file '", path,
            "' to export data"); //, ex.what());
        throw;
    }
}

void ExperimentExporter::writeData(const std::map<std::string, DataSet*> data)
{

    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    const std::string dataCollectionsKey = "/" + ohkl::gr_DataCollections;
    writeFrames(file, dataCollectionsKey, data);

    for (const auto& it : data) {
        DataSet* data_item = it.second;
        const std::string datakey = dataCollectionsKey + "/" + data_item->name();
        // Write detector states
        writeDetectorState(file, datakey, data_item);

        // Write sample states
        writeSampleState(file, datakey, data_item);

        // Write masks
        writeMasks(file, datakey, data_item);

        // Write all metadata (string, int and double) into the "Metadata" Group attributes
        const std::string metaKey = datakey + "/" + ohkl::gr_Metadata;
        H5::Group meta_group = file.createGroup(metaKey);
        writeMetadata(meta_group, data_item->metadata());
    }
}


void ExperimentExporter::writePeaks(const std::map<std::string, PeakCollection*> peakCollections)
{
    std::string peakCollectionsKey = "/" + ohkl::gr_PeakCollections;
    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    file.createGroup(peakCollectionsKey);

    for (const auto& it : peakCollections) {
        // Write the data
        const std::string collection_name = it.first;
        PeakCollection* const collection_item = it.second;
        const std::string collectionNameKey = peakCollectionsKey + "/" + collection_name;

        H5::Group peak_group{file.createGroup(collectionNameKey)};

        // initialize doubles
        const std::size_t nPeaks = collection_item->numberOfPeaks();
        // TODO: no peak_begin?
        std::vector<double> peak_end(nPeaks);
        std::vector<double> bkg_begin(nPeaks);
        std::vector<double> bkg_end(nPeaks);
        std::vector<double> scale(nPeaks);
        std::vector<double> transmission(nPeaks);
        std::vector<double> intensity(nPeaks);
        std::vector<double> sigma(nPeaks);
        std::vector<double> mean_bkg_val(nPeaks);
        std::vector<double> mean_bkg_sig(nPeaks);
        std::vector<double> mean_bkg_grad(nPeaks);
        std::vector<double> mean_bkg_grad_sigma(nPeaks);

        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> hkl_error(nPeaks, 3);
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> center(nPeaks, 3);
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> metric(
            3 * nPeaks, 3);
        Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> hkl(nPeaks, 3);

        center.setZero(nPeaks, 3);
        metric.setZero(3 * nPeaks, 3);

        // initialize integers
        std::vector<int> rejection_flag(nPeaks);

        // initialize the booleans
        std::unique_ptr<bool[]> selected{new bool[nPeaks]};
        std::unique_ptr<bool[]> masked{new bool[nPeaks]};
        std::unique_ptr<bool[]> predicted{new bool[nPeaks]};

        // initialize the datanames
        std::vector<std::string> data_names;
        std::vector<std::string> unit_cells;

        std::string name;
        std::string ext;

        for (std::size_t i = 0; i < nPeaks; ++i) {
            ohkl::Peak3D* const peak = collection_item->getPeak(i);

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
            mean_bkg_grad[i] = peak->meanBkgGradient().value();
            mean_bkg_grad_sigma[i] = peak->meanBkgGradient().sigma();

            rejection_flag[i] = static_cast<int>(peak->rejectionFlag());

            selected[i] = peak->selected();
            masked[i] = peak->masked();
            predicted[i] = peak->predicted();

            data_names.push_back(peak->dataSet()->name());

            const UnitCell* unit_cell_ptr = peak->unitCell();

            Eigen::Vector3d temp_col = peak->shape().center();
            center.block(i, 0, 1, 3) = Eigen::RowVector3d{temp_col(0), temp_col(1), temp_col(2)};
            metric.block(i * 3, 0, 3, 3) = peak->shape().metric();
            if (unit_cell_ptr) {
                const std::string unit_cell_id = std::to_string(unit_cell_ptr->id());
                unit_cells.push_back(unit_cell_id);

                temp_col = peak->hkl().error();
                hkl_error.block(i, 0, 1, 3) =
                    Eigen::RowVector3d(temp_col(0), temp_col(1), temp_col(2));

                Eigen::Vector3i itemp_col = peak->hkl().rowVector();
                hkl.block(i, 0, 1, 3) =
                    Eigen::RowVector3i(itemp_col(0), itemp_col(1), itemp_col(2));
            }
        }

        // TODO: explain! check size 2 vs 3!
        const hsize_t num_peaks[1] = {nPeaks};
        const H5::DataSpace peak_space(1, num_peaks);
        const hsize_t center_peaks_h[2] = {nPeaks, 3};
        const H5::DataSpace center_space(2, center_peaks_h);
        const hsize_t metric_peaks_h[2] = {nPeaks * 3, 3};
        const H5::DataSpace metric_space(2, metric_peaks_h);
        const H5::DataSpace hkl_error_space(2, center_peaks_h);
        const H5::DataSpace hkl_space(2, center_peaks_h);

        const std::vector<std::tuple<std::string, H5::DataType, H5::DataSpace, const void*>>
            peakData_defs{
                // NATIVE_DOUBLE
                {ohkl::ds_PeakEnd, H5::PredType::NATIVE_DOUBLE, peak_space, peak_end.data()},
                {ohkl::ds_BkgBegin, H5::PredType::NATIVE_DOUBLE, peak_space, bkg_begin.data()},
                {ohkl::ds_BkgEnd, H5::PredType::NATIVE_DOUBLE, peak_space, bkg_end.data()},
                {ohkl::ds_Scale, H5::PredType::NATIVE_DOUBLE, peak_space, scale.data()},
                {ohkl::ds_Transmission, H5::PredType::NATIVE_DOUBLE, peak_space,
                 transmission.data()},
                {ohkl::ds_Intensity, H5::PredType::NATIVE_DOUBLE, peak_space, intensity.data()},
                {ohkl::ds_Sigma, H5::PredType::NATIVE_DOUBLE, peak_space, sigma.data()},
                {ohkl::ds_BkgIntensity, H5::PredType::NATIVE_DOUBLE, peak_space,
                 mean_bkg_val.data()},
                {ohkl::ds_BkgSigma, H5::PredType::NATIVE_DOUBLE, peak_space, mean_bkg_sig.data()},
                {ohkl::ds_hklError, H5::PredType::NATIVE_DOUBLE, hkl_error_space, hkl_error.data()},
                {ohkl::ds_Center, H5::PredType::NATIVE_DOUBLE, center_space, center.data()},
                {ohkl::ds_Metric, H5::PredType::NATIVE_DOUBLE, metric_space, metric.data()},
                // NATIVE_INT32
                {ohkl::ds_Rejection, H5::PredType::NATIVE_INT32, peak_space, rejection_flag.data()},
                {ohkl::ds_hkl, H5::PredType::NATIVE_INT32, hkl_space, hkl.data()},
                // NATIVE_HBOOL
                {ohkl::ds_Selected, H5::PredType::NATIVE_HBOOL, peak_space, selected.get()},
                {ohkl::ds_Masked, H5::PredType::NATIVE_HBOOL, peak_space, masked.get()},
                {ohkl::ds_Predicted, H5::PredType::NATIVE_HBOOL, peak_space, predicted.get()},
                {ohkl::ds_bkgGrad, H5::PredType::NATIVE_DOUBLE, peak_space, mean_bkg_grad.data()},
                {ohkl::ds_bkgGradSigma, H5::PredType::NATIVE_DOUBLE, peak_space,
                 mean_bkg_grad_sigma.data()}};

        for (const auto& [dkey, dtype, dspace, dptr] : peakData_defs) {
            H5::DataSet data_H5(file.createDataSet(collectionNameKey + "/" + dkey, dtype, dspace));
            data_H5.write(dptr, dtype, dspace, dspace);
        }

        // Write DataNames
        writePeakDataNames(file, collectionNameKey + "/" + ohkl::ds_DatasetNames, data_names);
        // Write unit-cell names
        writePeakDataNames(file, collectionNameKey + "/" + ohkl::ds_UnitCellNames, unit_cells);

        // Write peak metadata as attributes of the peak group
        writeMetadata(peak_group, collection_item->metadata());
    }
}

void ExperimentExporter::writeUnitCells(const std::vector<UnitCell*> unit_cells)
{
    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    file.createGroup("/" + ohkl::gr_UnitCells);
    H5::DataSpace metaSpace(H5S_SCALAR);
    H5::StrType str80Type(H5::PredType::C_S1, 80);

    for (const auto* unit_cell : unit_cells) {
        // Write the data
        const unsigned int unit_cell_id = unit_cell->id();

        const Eigen::MatrixX3d rec = unit_cell->reciprocalBasis();

        const std::string group_name =
            "/" + ohkl::gr_UnitCells + "/" + std::to_string(unit_cell_id);
        H5::Group unit_cell_group = file.createGroup(group_name);

        // Write reciprocal-vector components
        for (std::size_t i = 0; i < 3; ++i) {
            for (std::size_t j = 0; j < 3; ++j) {
                std::stringstream key_ss;
                key_ss << (ohkl::at_rVec + "_") << i << j; // eg., "reciprocal_01"
                writeAttribute(
                    unit_cell_group, key_ss.str(), &rec(i, j), H5::PredType::NATIVE_DOUBLE,
                    metaSpace);
            }
        }

        const std::string unit_cell_name = unit_cell->name();
        writeAttribute(
            unit_cell_group, ohkl::at_unitCellName, unit_cell_name.data(), str80Type, metaSpace);
        const double tolerance = unit_cell->indexingTolerance();
        writeAttribute(
            unit_cell_group, ohkl::at_indexingTol, &tolerance, H5::PredType::NATIVE_DOUBLE,
            metaSpace);
        const std::string bravais_type_sym = unit_cell->bravaisTypeSymbol();
        writeAttribute(
            unit_cell_group, ohkl::at_BravaisLattice, bravais_type_sym.data(), str80Type,
            metaSpace);
        const std::string unitcell_spacegroup_sym = unit_cell->spaceGroup().symbol();
        writeAttribute(
            unit_cell_group, ohkl::at_spacegroup, unitcell_spacegroup_sym.data(), str80Type,
            metaSpace);
        const unsigned int z_val = unit_cell->z();
        writeAttribute(unit_cell_group, ohkl::at_z, &z_val, H5::PredType::NATIVE_UINT, metaSpace);
    }
}

void ExperimentExporter::writeInstrumentStates(
    std::map<DataSet*, ohkl::InstrumentStateSet*> instrumentStateMap)
{
    std::string instrumentStateKey = "/" + ohkl::gr_Instrument;
    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    file.createGroup(instrumentStateKey);

    for (const auto& [dataset, instrumentStateSet] : instrumentStateMap) {
        std::string instrumentStateSetKey = instrumentStateKey + "/" + dataset->name();
        H5::Group instrument_grp{file.createGroup(instrumentStateSetKey)};
        const ohkl::InstrumentStateList& instrumentStates = instrumentStateSet->instrumentStates();
        const std::size_t n_states = instrumentStates.size();
        //-- write detector orientation (3d matrix)
        const hsize_t mat3d_size = 9; // 3x3=9 matrix elements
        const hsize_t quaternion_size = 4; // x,y,z,w = 4 elements
        const hsize_t vec3d_size = 3; // 3 elements

        const H5::DataType stateValueType{H5::PredType::NATIVE_DOUBLE};
        HDF5TableWriter detectorOrientation(
            instrument_grp, stateValueType, n_states, mat3d_size, ohkl::ds_detectorOrientation),
            detectorPositionOffset(
                instrument_grp, stateValueType, n_states, vec3d_size,
                ohkl::ds_detectorPositionOffset),
            sampleOrientation(
                instrument_grp, stateValueType, n_states, quaternion_size,
                ohkl::ds_sampleOrientation),
            sampleOrientationOffset(
                instrument_grp, stateValueType, n_states, quaternion_size,
                ohkl::ds_sampleOrientationOffset),
            samplePosition(
                instrument_grp, stateValueType, n_states, vec3d_size, ohkl::ds_samplePosition),
            ni(instrument_grp, stateValueType, n_states, vec3d_size, ohkl::ds_beamDirection),
            wavelength(instrument_grp, stateValueType, n_states, 1, ohkl::ds_beamWavelength),
            refined(
                instrument_grp, H5::PredType::NATIVE_HBOOL, n_states, 1, ohkl::ds_isRefinedState);

        std::size_t i_row = 0;
        for (const ohkl::InstrumentState& state : instrumentStates) {
            detectorOrientation.writeRow(i_row, state.detectorOrientation.data());
            detectorPositionOffset.writeRow(i_row, state.detectorPositionOffset.data());
            samplePosition.writeRow(i_row, state.samplePosition.data());
            sampleOrientation.writeRow(i_row, state.sampleOrientation.coeffs().data());
            sampleOrientationOffset.writeRow(i_row, state.sampleOrientationOffset.coeffs().data());
            samplePosition.writeRow(i_row, state.samplePosition.data());
            ni.writeRow(i_row, state.ni.data());
            wavelength.writeRow(i_row, &state.wavelength);
            refined.writeRow(i_row, &state.refined);

            i_row += 1;
        }
    }
}

void ExperimentExporter::finishWrite()
{
    // _file.close();
}

} // namespace ohkl
