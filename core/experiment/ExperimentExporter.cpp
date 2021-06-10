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

#include "H5Cpp.h"

namespace {
// HDF5 DataTypes
static const H5::StrType str80(H5::PredType::C_S1, 80);
static const H5::StrType metaStrType(H5::PredType::C_S1, 80);  // TODO: why fixed length, and not `H5T_VARIABLE`?

// HDF5 DataSpace (defining data shape)
static const H5::DataSpace metaExpSpace(H5S_SCALAR);
static const H5::DataSpace metaSpace(H5S_SCALAR);

// write functions
inline
void writeAttribute(H5::H5File& file, const std::string& key, const void* const value,
		    const H5::DataType& datatype, const H5::DataSpace& dataspace)
{
    H5::Attribute attr(file.createAttribute(key, datatype, dataspace));
    attr.write(datatype, value);
}

} // namespace


namespace nsx {

void ExperimentExporter::createFile(std::string name, std::string diffractometer, std::string path)
{
    H5::H5File file{path.c_str(), H5F_ACC_TRUNC};
    _file_name = path;  // store the filename for later use

    writeAttribute(file, "name", name.data(), str80, metaExpSpace);
    writeAttribute(file, "diffractometer", diffractometer.data(), str80, metaExpSpace);
}

void ExperimentExporter::writeData(const std::map<std::string, DataSet*> data)
{
    const std::string dataCollectionsKey = "/DataCollections";

    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    file.createGroup(dataCollectionsKey);

    for (const auto& it : data) {
        const DataSet* data_item = it.second;
        const std::string name = data_item->name();
        const std::string datakey = dataCollectionsKey + "/" + name;


        blosc_init();
        blosc_set_nthreads(4);

        hsize_t dims[3] = {data_item->nFrames(), data_item->nRows(), data_item->nCols()};
        hsize_t chunk[3] = {1, data_item->nRows(), data_item->nCols()};
        hsize_t count[3] = {1, data_item->nRows(), data_item->nCols()};

        H5::DSetCreatPropList plist;
        plist.setChunk(3, chunk);
        char *version, *date;
        int r;
        unsigned int cd_values[7];
        cd_values[4] = 9; // Highest compression level
        cd_values[5] = 1; // Bit shuffling active
        cd_values[6] = BLOSC_BLOSCLZ; // Seem to be the best compromise
        // speed/compression for diffraction data

        r = register_blosc(&version, &date);
        if (r <= 0)
        H5::Group data_collection{file.createGroup(datakey)};

            throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

        // caught by valgrind memcheck
        free(version);
        version = nullptr;
        free(date);
        date = nullptr;
        plist.setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);

	// DataSet for frames
	const hsize_t dims[3] = {n_frames, n_rows, n_cols};
        const H5::DataSpace space(3, dims, nullptr);
	const H5::DataType frameType {H5::PredType::NATIVE_INT32};
        H5::DataSet dset(file.createDataSet(std::string(datakey + "/" + name),
                                            frameType, space,
                                            plist));

        hsize_t offset[3];
        offset[0] = 0;
        offset[1] = 0;
        offset[2] = 0;

        // write frames
        H5::DataSpace memspace(3, count, nullptr);
        using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
        for (offset[0] = 0; offset[0] < data_item->nFrames(); offset[0] += count[0]) {
            space.selectHyperslab(H5S_SELECT_SET, count, offset, nullptr, nullptr);
            // HDF5 requires row-major storage, so copy frame into a row-major matrix
            IntMatrix current_frame(data_item->frame(offset[0]));
            dset.write(current_frame.data(), H5::PredType::NATIVE_INT32, memspace, space);
        }

        blosc_destroy();

        hsize_t nf[1] = {data_item->nFrames()};
        H5::DataSpace scanSpace(1, nf);

        const auto& detectorStates = data_item->reader()->detectorStates();
        const auto& detector_gonio = data_item->reader()->diffractometer()->detector()->gonio();
        size_t n_detector_gonio_axes = detector_gonio.nAxes();
        for (size_t i = 0; i < n_detector_gonio_axes; ++i) {
            const auto& axis = detector_gonio.axis(i);
            Eigen::VectorXd values(data_item->nFrames());
            for (size_t j = 0; j < data_item->nFrames(); ++j)
                values(j) = detectorStates[j][i] / deg;
	const std::string detectorKey = datakey + "/Detector";
        file.createGroup(detectorKey);
            H5::DataSet detector_scan(file.createDataSet(
                std::string(detectorKey + "/" + axis.name()),
                stateValueType, scanSpace));
        }


        const auto& sampleStates = data_item->reader()->sampleStates();
        const auto& sample_gonio = data_item->reader()->diffractometer()->sample().gonio();
        size_t n_sample_gonio_axes = sample_gonio.nAxes();

        for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
            const auto& axis = sample_gonio.axis(i);
            Eigen::VectorXd values(data_item->nFrames());
            for (size_t j = 0; j < data_item->nFrames(); ++j)
                values(j) = sampleStates[j][i] / deg;
        // Write sample states  // TODO: move to a separate function
	const std::string sampleKey = datakey + "/Sample";
        file.createGroup(sampleKey);
            H5::DataSet sample_scan(file.createDataSet(
                std::string(sampleKey + "/" + axis.name()),
                stateValueType, scanSpace));
            sample_scan.write(&values(0), stateValueType, scanSpace, scanSpace);
        }

        // Write all string metadata into the "Info" group
        H5::Group info_group = file.createGroup(std::string("/DataCollections/" + name + "/Info"));

        H5::DataSpace metaSpace(H5S_SCALAR);
        H5::StrType str80(H5::PredType::C_S1, 80);  // TODO: why fixed length, and not `H5T_VARIABLE`?
        std::string info;

        const nsx::MetaDataMap& map = data_item->metadata().map();

        for (const auto& item : map) {
            std::string info;

            // TODO: why using `try..catch` instead of `std::has_alternative` and `std::get` for the Variant
            try {
                info = std::get<std::string>(item.second);
                H5::Attribute intAtt(info_group.createAttribute(item.first, str80, metaSpace));
                intAtt.write(str80, info);
            } catch (const std::exception& ex) {
                nsxlog(Level::Debug, "Exception in", __PRETTY_FUNCTION__, ":", ex.what());
            } catch (...) {
                nsxlog(Level::Error, "Uncaught exception in", __PRETTY_FUNCTION__);
            }
        }

        // Write all other metadata (int and double) into the "Experiment" Group
        H5::Group meta_group = file.createGroup(std::string("/DataCollections/" + name + "/Meta"));

        for (const auto& item : map) {
            int value;

            // TODO: why using `try..catch` instead of `std::has_alternative` and `std::get` for the Variant
            try {
                value = std::get<int>(item.second);
                H5::Attribute intAtt(
                    meta_group.createAttribute(item.first, H5::PredType::NATIVE_INT32, metaSpace));
                intAtt.write(H5::PredType::NATIVE_INT, &value);
            } catch (...) {
                try {
                    double d_value;
                    d_value = std::get<double>(item.second);
                    H5::Attribute intAtt(meta_group.createAttribute(
                        item.first, H5::PredType::NATIVE_DOUBLE, metaSpace));
                    intAtt.write(H5::PredType::NATIVE_DOUBLE, &d_value);
                } catch (const std::exception& ex) {
                    nsxlog(Level::Debug, "Exception in", __PRETTY_FUNCTION__, ":", ex.what());
                } catch (...) {
                    nsxlog(Level::Error, "Uncaught exception in", __PRETTY_FUNCTION__);
                }
            }
        }
    }
}


void ExperimentExporter::writeInstrument(const Diffractometer*)
{
    // TODO ...
}

void ExperimentExporter::writePeaks(const std::map<std::string, PeakCollection*> peakCollections)
{
    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    file.createGroup("/PeakCollections");

    for (const auto& it : peakCollections) {
        // Write the data
        std::string collection_name = it.first;
        PeakCollection* collection_item = it.second;

        file.createGroup(std::string("/PeakCollections/" + collection_name));

        // initialize doubles
        const int nPeaks = collection_item->numberOfPeaks();
        double* peak_end = new double[nPeaks];
        double* bkg_begin = new double[nPeaks];
        double* bkg_end = new double[nPeaks];
        double* scale = new double[nPeaks];
        double* transmission = new double[nPeaks];
        double* intensity = new double[nPeaks];
        double* sigma = new double[nPeaks];
        double* mean_bkg_val = new double[nPeaks];
        double* mean_bkg_sig = new double[nPeaks];

        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> center(nPeaks, 3);
        Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> metric(
            3 * nPeaks, 3);

        center.setZero(nPeaks, 3);
        metric.setZero(3 * nPeaks, 3);

        // initialize integers
        int* rejection_flag = new int[nPeaks];

        // initialize the booleans
        bool* selected = new bool[nPeaks];
        bool* masked = new bool[nPeaks];
        bool* predicted = new bool[nPeaks];

        // initialize the datanames
        std::vector<std::string> data_names;
        std::vector<std::string> unit_cells;

        std::string name;
        std::string ext;
        std::string temp = "NONE";
        std::string unit_cell_name;

        for (int i = 0; i < nPeaks; ++i) {
            const nsx::Peak3D* peak = collection_item->getPeak(i);

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

            selected[i] = peak->selected();
            masked[i] = peak->masked();
            predicted[i] = peak->predicted();

            data_names.push_back(peak->dataSet()->name());

            const UnitCell* unit_cell_ptr = peak->unitCell();
            if (unit_cell_ptr) {
                unit_cell_name = unit_cell_ptr->name();
                unit_cells.push_back(unit_cell_name);
            } else {
                unit_cells.push_back(temp);
            }

            Eigen::Vector3d temp_col = peak->shape().center();
            center.block(i, 0, 1, 3) = Eigen::RowVector3d{temp_col(0), temp_col(1), temp_col(2)};
            metric.block(i * 3, 0, 3, 3) = peak->shape().metric();
        }

        // TODO: explain! check size 2 vs 3!
        hsize_t num_peaks[1] = {static_cast<unsigned long long>(nPeaks)};
        H5::DataSpace peak_space(1, num_peaks);
        hsize_t center_peaks_h[2] = {static_cast<unsigned long long>(nPeaks), 3};
        H5::DataSpace center_space(2, center_peaks_h);
        hsize_t metric_peaks_h[2] = {static_cast<unsigned long long>(nPeaks) * 3, 3};
        H5::DataSpace metric_space(2, metric_peaks_h);

        H5::DataSet peak_end_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/PeakEnd", H5::PredType::NATIVE_DOUBLE,
            peak_space));
        peak_end_H5.write(peak_end, H5::PredType::NATIVE_DOUBLE, peak_space, peak_space);

        H5::DataSet bkg_begin_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/BkgBegin", H5::PredType::NATIVE_DOUBLE,
            peak_space));
        bkg_begin_H5.write(bkg_begin, H5::PredType::NATIVE_DOUBLE, peak_space, peak_space);

        H5::DataSet bkg_end_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/BkgEnd", H5::PredType::NATIVE_DOUBLE,
            peak_space));
        bkg_end_H5.write(bkg_end, H5::PredType::NATIVE_DOUBLE, peak_space, peak_space);

        H5::DataSet scale_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Scale", H5::PredType::NATIVE_DOUBLE,
            peak_space));
        scale_H5.write(scale, H5::PredType::NATIVE_DOUBLE, peak_space, peak_space);

        H5::DataSet transmission_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Transmission", H5::PredType::NATIVE_DOUBLE,
            peak_space));
        transmission_H5.write(transmission, H5::PredType::NATIVE_DOUBLE, peak_space, peak_space);

        H5::DataSet intensity_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Intensity", H5::PredType::NATIVE_DOUBLE,
            peak_space));
        intensity_H5.write(intensity, H5::PredType::NATIVE_DOUBLE, peak_space, peak_space);

        H5::DataSet sigma_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Sigma", H5::PredType::NATIVE_DOUBLE,
            peak_space));
        sigma_H5.write(sigma, H5::PredType::NATIVE_DOUBLE, peak_space, peak_space);

        H5::DataSet mean_bkg_val_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/BkgIntensity", H5::PredType::NATIVE_DOUBLE,
            peak_space));
        mean_bkg_val_H5.write(mean_bkg_val, H5::PredType::NATIVE_DOUBLE, peak_space, peak_space);

        H5::DataSet mean_bkg_sig_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/BkgSigma", H5::PredType::NATIVE_DOUBLE,
            peak_space));
        mean_bkg_sig_H5.write(mean_bkg_sig, H5::PredType::NATIVE_DOUBLE, peak_space, peak_space);

        H5::DataSet rejection_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Rejection", H5::PredType::NATIVE_INT32,
            peak_space));
        rejection_H5.write(rejection_flag, H5::PredType::NATIVE_INT32, peak_space, peak_space);

        H5::DataSet center_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Center", H5::PredType::NATIVE_DOUBLE,
            center_space));
        center_H5.write(center.data(), H5::PredType::NATIVE_DOUBLE, center_space, center_space);

        H5::DataSet metric_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Metric", H5::PredType::NATIVE_DOUBLE,
            metric_space));
        metric_H5.write(metric.data(), H5::PredType::NATIVE_DOUBLE, metric_space, metric_space);

        H5::DataSet selected_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Selected", H5::PredType::NATIVE_HBOOL,
            peak_space));
        selected_H5.write(selected, H5::PredType::NATIVE_HBOOL, peak_space, peak_space);

        H5::DataSet masked_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Masked", H5::PredType::NATIVE_HBOOL,
            peak_space));
        masked_H5.write(masked, H5::PredType::NATIVE_HBOOL, peak_space, peak_space);

        H5::DataSet predicted_H5(file.createDataSet(
            "/PeakCollections/" + collection_name + "/Predicted", H5::PredType::NATIVE_HBOOL,
            peak_space));
        predicted_H5.write(predicted, H5::PredType::NATIVE_HBOOL, peak_space, peak_space);

        {
            std::vector<const char*> data_name_pointers(nPeaks);
            for (int i = 0; i < nPeaks; ++i)
                data_name_pointers[i] = data_names[i].c_str();
            H5::StrType data_str_type(H5::PredType::C_S1, H5T_VARIABLE);
            H5::DataSet data_H5(file.createDataSet(
                "/PeakCollections/" + collection_name + "/DataNames", data_str_type, peak_space));
            data_H5.write(data_name_pointers.data(), data_str_type, peak_space, peak_space);
        }

        {
            std::vector<const char*> unit_cell_pointers(nPeaks);
            for (int i = 0; i < nPeaks; ++i)
                unit_cell_pointers[i] = unit_cells[i].c_str();
            H5::StrType uc_str_type(H5::PredType::C_S1, H5T_VARIABLE);
            H5::DataSet unit_cell_H5(file.createDataSet(
                "/PeakCollections/" + collection_name + "/UnitCells", uc_str_type, peak_space));
            unit_cell_H5.write(unit_cell_pointers.data(), uc_str_type, peak_space, peak_space);
        }

        // Write all other metadata (int and double) into the "Experiment" Group
        H5::Group meta_peak_group(
            file.createGroup("/PeakCollections/" + collection_name + "/Meta"));

        std::map<std::string, float>* map = collection_item->meta(); // TODO: Bad variable name `map`
        H5::DataSpace metaSpace(H5S_SCALAR);
        H5::StrType str80(H5::PredType::C_S1, 80);

        for (const auto& item : *map) {
            int value;
            try {
                value = item.second;
                H5::Attribute intAtt(meta_peak_group.createAttribute(
                    item.first, H5::PredType::NATIVE_INT32, metaSpace));
                intAtt.write(H5::PredType::NATIVE_INT32, &value);
            } catch (const std::exception& ex) {
                nsxlog(Level::Debug, "Exception in", __PRETTY_FUNCTION__, ":", ex.what());
            } catch (...) {
                nsxlog(Level::Error, "Uncaught exception in", __PRETTY_FUNCTION__);
            }
        }

        H5::Attribute type_att(
            meta_peak_group.createAttribute("Type", H5::PredType::NATIVE_INT32, metaSpace));
        int listtype_int = static_cast<int>(collection_item->type());
        type_att.write(H5::PredType::NATIVE_INT32, &listtype_int);

        delete[] peak_end;
        delete[] bkg_begin;
        delete[] bkg_end;
        delete[] scale;
        delete[] transmission;
        delete[] intensity;
        delete[] sigma;
        delete[] mean_bkg_val;
        delete[] mean_bkg_sig;

        delete[] rejection_flag;

        delete[] selected;
        delete[] masked;
        delete[] predicted;
    }
}

void ExperimentExporter::writeUnitCells(const std::map<std::string, UnitCell*> unit_cells)
{
    H5::H5File file{_file_name.c_str(), H5F_ACC_RDWR};
    file.createGroup("/UnitCells");
    H5::DataSpace metaSpace(H5S_SCALAR);
    H5::StrType str80(H5::PredType::C_S1, 80);

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
        H5::Attribute bravais(unit_cell_group.createAttribute("bravais", str80, metaSpace));
        H5::Attribute space_group(unit_cell_group.createAttribute("space_group", str80, metaSpace));
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
        bravais.write(str80, unit_cell->bravaisTypeSymbol());
        space_group.write(str80, unit_cell->spaceGroup().symbol());
        z.write(H5::PredType::NATIVE_UINT, &(z_val));
    }
}

void ExperimentExporter::finishWrite()
{
    // _file.close();
}

} // namespace nsx
