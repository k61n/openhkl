//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/experiment/ExperimentImporter.cpp
//! @brief     Imports the Experiment saved by ExperimentExporter
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/experiment/ExperimentImporter.h"
#include "base/geometry/Ellipsoid.h"
#include "base/utils/Logger.h"
#include "core/data/DataSet.h"
#include "core/data/DataTypes.h"
#include "core/experiment/Experiment.h"
#include "core/loader/HDF5DataReader.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"
#include "tables/crystal/MillerIndex.h"
#include <Eigen/Dense>

#include <H5Cpp.h>
#include <stdexcept>
#include <string>

namespace nsx {

void ExperimentImporter::setFilePath(const std::string path, Experiment* const experiment)
{
    nsxlog(nsx::Level::Debug, "Importing experiment info from path '", path, "'");

    try {
        _file_name = path;
        H5::H5File file(_file_name.c_str(), H5F_ACC_RDONLY);
        if (file.attrExists(nsx::at_experiment)) {
            const H5::Attribute attr = file.openAttribute(nsx::at_experiment);
            const H5::DataType attr_type = attr.getDataType();
            std::string value;
            attr.read(attr_type, value);
            experiment->setName(value);
        }

        if (file.attrExists(nsx::at_diffractometer)) {
            const H5::Attribute attr = file.openAttribute(nsx::at_diffractometer);
            const H5::DataType attr_type = attr.getDataType();
            std::string value;
            attr.read(attr_type, value);
            experiment->setDiffractometer(value);
        }

        if (file.attrExists(nsx::at_nsxVersion)) {
            const H5::Attribute attr = file.openAttribute(nsx::at_nsxVersion);
            const H5::DataType attr_type = attr.getDataType();
            std::string value;
            attr.read(attr_type, value);
            nsxlog(Level::Info, "Loading data NSXTool version ", value);
        }

        if (file.attrExists(nsx::at_commitHash)) {
            const H5::Attribute attr = file.openAttribute(nsx::at_commitHash);
            const H5::DataType attr_type = attr.getDataType();
            std::string value;
            attr.read(attr_type, value);
            nsxlog(Level::Info, "Git commit hash ", value);
        }

        nsxlog(
            nsx::Level::Info, "Finished importing info for Experiment '" + experiment->name() + "'",
            " with diffractometer '" + experiment->getDiffractometer()->name() + "'",
            " from path '" + path + "'");

    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }
}

void ExperimentImporter::loadData(Experiment* experiment)
{
    nsxlog(nsx::Level::Debug, "Importing data from file '", _file_name, "'");

    try {
        H5::H5File file(_file_name.c_str(), H5F_ACC_RDONLY);
        H5::Group data_collections(file.openGroup(nsx::gr_DataCollections));

        hsize_t object_num = data_collections.getNumObjs();
        for (int i = 0; i < object_num; ++i) {
            const std::string collection_name = data_collections.getObjnameByIdx(i);
            const nsx::sptrDataSet dataset_ptr{
                std::make_shared<nsx::DataSet>(collection_name, experiment->getDiffractometer())};
            dataset_ptr->addDataFile(_file_name, "nsx");
            dataset_ptr->finishRead();
            experiment->addData(dataset_ptr);
        }
    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }

    nsxlog(nsx::Level::Debug, "Finished importing data from file '", _file_name, "'");
}

void ExperimentImporter::loadPeaks(Experiment* experiment)
{
    nsxlog(nsx::Level::Debug, "Importing peaks from file '", _file_name, "'");

    using Eigen_VecXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::RowMajor>;
    using Eigen_VecXint = Eigen::Matrix<int, Eigen::Dynamic, Eigen::RowMajor>;
    using Eigen_VecXbool = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::RowMajor>;

    try {
        H5::H5File file(_file_name.c_str(), H5F_ACC_RDONLY);
        H5::Group peak_collections(file.openGroup(nsx::gr_PeakCollections));

        hsize_t object_num = peak_collections.getNumObjs();
        for (int i = 0; i < object_num; ++i) {
            const std::string collection_name = peak_collections.getObjnameByIdx(i);
            const std::string collection_key = nsx::gr_PeakCollections + "/" + collection_name;
            H5::Group peak_collection(file.openGroup(collection_key));

            // Read the info group and store in metadata
            int n_peaks = 0;
            int type = 0;

            std::string indexed;
            std::string integrated;

            if (peak_collection.attrExists(nsx::at_peakCount)) {
                const H5::Attribute attr = peak_collection.openAttribute(nsx::at_peakCount);
                const H5::DataType attr_type = attr.getDataType();
                attr.read(attr_type, &n_peaks);
            }

            if (peak_collection.attrExists(nsx::at_peakType)) {
                const H5::Attribute attr = peak_collection.openAttribute(nsx::at_peakType);
                const H5::DataType attr_type = attr.getDataType();
                attr.read(attr_type, &type);
            }


            if (peak_collection.attrExists(nsx::at_indexed)) {
                const H5::Attribute attr = peak_collection.openAttribute(nsx::at_indexed);
                const H5::DataType attr_type = attr.getDataType();
                attr.read(attr_type, &indexed);
            }

            if (peak_collection.attrExists(nsx::at_integrated)) {
                const H5::Attribute attr = peak_collection.openAttribute(nsx::at_integrated);
                const H5::DataType attr_type = attr.getDataType();
                attr.read(attr_type, &integrated);
            }


            nsxlog(
                Level::Debug, "ExperimentImporter::loadPeaks: found ", n_peaks,
                " to import for PeakCollection '", collection_name, "'");
            nsxlog(Level::Debug, "Preparing the dataspace");

            // prepare the loading
            Eigen_VecXd bkg_begin(n_peaks);
            Eigen_VecXd bkg_end(n_peaks);
            Eigen_VecXd peak_end(n_peaks);
            Eigen_VecXd scale(n_peaks);
            Eigen_VecXd transmission(n_peaks);
            Eigen_VecXd intensity(n_peaks);
            Eigen_VecXd sigma(n_peaks);
            Eigen_VecXd mean_bkg_val(n_peaks);
            Eigen_VecXd mean_bkg_sig(n_peaks);

            Eigen_VecXint rejection_flag(n_peaks);

            std::map<std::string, Eigen_VecXd*> double_keys{
                {nsx::ds_BkgBegin, &bkg_begin},
                {nsx::ds_BkgEnd, &bkg_end},
                {nsx::ds_PeakEnd, &peak_end},
                {nsx::ds_Scale, &scale},
                {nsx::ds_Transmission, &transmission},
                {nsx::ds_Intensity, &intensity},
                {nsx::ds_Sigma, &sigma},
                {nsx::ds_BkgIntensity, &mean_bkg_val},
                {nsx::ds_BkgSigma, &mean_bkg_sig}};

            std::map<std::string, Eigen_VecXint*> int_keys{
                {nsx::ds_Rejection, &rejection_flag},};

            Eigen_VecXbool predicted(n_peaks);
            Eigen_VecXbool masked(n_peaks);
            Eigen_VecXbool selected(n_peaks);

            std::map<std::string, Eigen_VecXbool*> bool_keys{
                {nsx::ds_Predicted, &predicted},
                {nsx::ds_Masked, &masked},
                {nsx::ds_Selected, &selected}};

            Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> center(
                n_peaks, 3);
            Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> metric(
                3 * n_peaks, 3);
            Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> hkl_error(
                n_peaks, 3);

            Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> hkl(
                n_peaks, 3);

            std::vector<std::string> data_names;
            std::vector<unsigned int> unit_cells;

            nsxlog(Level::Debug, "Importing doubles");
            // Load all doubles
            for (const auto& [key, val] : double_keys) {
                H5::DataSet data_set = peak_collection.openDataSet(key);
                H5::DataSpace space(data_set.getSpace());
                data_set.read(val->data(), H5::PredType::NATIVE_DOUBLE, space, space);
            };

            nsxlog(Level::Debug, "Importing integers");
            // Load all ints
            for (const auto& [key, val] : int_keys) {
                H5::DataSet data_set = peak_collection.openDataSet(key);
                H5::DataSpace space(data_set.getSpace());
                data_set.read(val->data(), H5::PredType::NATIVE_INT, space, space);
            }

            nsxlog(Level::Debug, "Importing booleans");
            // Load all booleans
            for (const auto& [key, val] : bool_keys) {
                H5::DataSet data_set = peak_collection.openDataSet(key);
                H5::DataSpace space(data_set.getSpace());
                data_set.read(val->data(), H5::PredType::NATIVE_HBOOL, space, space);
            }

            nsxlog(Level::Debug, "Importing centers");
            // Load the centers
            {
                H5::DataSet data_set = peak_collection.openDataSet(nsx::ds_Center);
                H5::DataSpace space(data_set.getSpace());
                data_set.read(center.data(), H5::PredType::NATIVE_DOUBLE, space, space);
            }

            nsxlog(Level::Debug, "Importing metric");
            // Load the metrics
            {
                H5::DataSet data_set = peak_collection.openDataSet(nsx::ds_Metric);
                H5::DataSpace space(data_set.getSpace());
                data_set.read(metric.data(), H5::PredType::NATIVE_DOUBLE, space, space);
            }

            nsxlog(Level::Debug, "Importing hkls");
            // Load the hkls
            {
                H5::DataSet data_set = peak_collection.openDataSet(nsx::ds_hkl);
                H5::DataSpace space(data_set.getSpace());
                data_set.read(hkl.data(), H5::PredType::NATIVE_INT, space, space);
            }

            nsxlog(Level::Debug, "Importing hkl errors");
            // Load the hkl errors
            {
                H5::DataSet data_set = peak_collection.openDataSet(nsx::ds_hklError);
                H5::DataSpace space(data_set.getSpace());
                data_set.read(hkl_error.data(), H5::PredType::NATIVE_DOUBLE, space, space);
            }

            nsxlog(Level::Debug, "Importing DataSet names");
            // Load the data_names
            {
                H5::DataSet data_set = peak_collection.openDataSet(nsx::ds_DatasetNames);
                H5::DataType data_type = data_set.getDataType();
                H5::DataSpace space(data_set.getSpace());

                hsize_t data_dims_out[1];
                space.getSimpleExtentDims(data_dims_out, nullptr);
                const hsize_t dataset_nr = data_dims_out[0]; // nr of datasets

                std::unique_ptr<char* []> char_data_names { new char*[dataset_nr] };
                data_set.read(char_data_names.get(), data_type);

                for (std::size_t ii = 0; ii < dataset_nr; ++ii) {
                    const std::string dataset_name{char_data_names[ii]};
                    data_names.push_back(dataset_name);
                }
            }

            nsxlog(Level::Debug, "Importing UnitCell names");
            // Load the unit cell strings
            if (experiment->numUnitCells() > 0) {
                H5::DataSet uc_data_set = peak_collection.openDataSet(nsx::ds_UnitCellNames);
                H5::DataType uc_data_type = uc_data_set.getDataType();
                H5::DataSpace uc_space = uc_data_set.getSpace();

                hsize_t uc_dims_out[1];
                uc_space.getSimpleExtentDims(uc_dims_out, nullptr);
                const hsize_t uc_nr = uc_dims_out[0]; // nr of unit cells

                std::unique_ptr<char* []> char_unit_cells { new char*[uc_nr] };
                uc_data_set.read(char_unit_cells.get(), uc_data_type);

                for (int ii = 0; ii < uc_nr; ++ii) {
                    const std::string uc_name{char_unit_cells[ii]};
                    unit_cells.push_back(std::stoi(uc_name));
                }
            }

            nsxlog(Level::Debug, "Finished reading peak data from file '", _file_name, "'");
            nsxlog(Level::Debug, "Creating the vector of peaks");
            std::vector<nsx::Peak3D*> peaks;

            Eigen::Vector3d local_center;
            Eigen::Matrix3d local_metric;
            Eigen::Vector3d local_hkl_error;
            Eigen::Vector3i local_hkl;

            for (int k = 0; k < n_peaks; ++k) {
                local_center = Eigen::Vector3d(center(k, 0), center(k, 1), center(k, 2));
                local_metric = metric.block(k * 3, 0, 3, 3);

                const nsx::Ellipsoid ellipsoid(local_center, local_metric);

                sptrDataSet data_pointer = experiment->getData(std::string(data_names[k]));
                nsx::Peak3D* peak = new nsx::Peak3D(data_pointer, ellipsoid);

                const nsx::Intensity peak_intensity(intensity[k], sigma[k]);
                const nsx::Intensity peak_mean_bkg(mean_bkg_val[k], mean_bkg_sig[k]);

                peak->setManually(
                    peak_intensity, peak_end[k], bkg_begin[k], bkg_end[k], scale[k],
                    transmission[k], peak_mean_bkg, predicted[k], selected[k], masked[k],
                    rejection_flag[k]);


                if (experiment->numUnitCells() > 0) {
                    local_hkl_error = {hkl_error(k, 0), hkl_error(k, 1), hkl_error(k, 2)};
                    local_hkl = {hkl(k, 0), hkl(k, 1), hkl(k, 2)};
                    const nsx::MillerIndex miller(local_hkl, local_hkl_error);
                    peak->setUnitCell(experiment->getSptrUnitCell(unit_cells[k]));
                    peak->setMillerIndices(miller);
                }

                peaks.push_back(peak);
            }
            nsxlog(Level::Debug, "Finished creating the vector of peaks");

            listtype collection_type = static_cast<listtype>(type);

            // converting data types. Ascii code of '0' is 48, of '1' is 49
            experiment->addPeakCollection(
                collection_name, collection_type, peaks, static_cast<bool>(indexed[0] - 48),
                static_cast<bool>(integrated[0] - 48));

            nsxlog(Level::Debug, "Finished creating the peak collection");
        }
    } catch (H5::Exception& e) {
        throw std::runtime_error{e.getDetailMsg()};
    }

    nsxlog(nsx::Level::Debug, "Finished importing peaks from file '", _file_name, "'");
}

void ExperimentImporter::loadUnitCells(Experiment* experiment)
{
    nsxlog(Level::Debug, "Importing unit cells from file '", _file_name, "'");

    try {
        H5::H5File file(_file_name.c_str(), H5F_ACC_RDONLY);
        H5::Group unit_cells(file.openGroup(nsx::gr_UnitCells));

        std::vector<unsigned int> cell_ids;

        const hsize_t object_num = unit_cells.getNumObjs();
        for (int i = 0; i < object_num; ++i) {
            double rec_00 = 1;
            double rec_01 = 0;
            double rec_02 = 0;
            double rec_10 = 0;
            double rec_11 = 1;
            double rec_12 = 0;
            double rec_20 = 0;
            double rec_21 = 0;
            double rec_22 = 1;
            double indexing_tolerance = 0.2;
            uint z = 1;
            std::string bravais = "aP";
            std::string space_group = "P 1";
            std::string unit_cell_name;

            if (!unit_cells.nameExists(std::to_string(i)))
                continue;
            H5::Group unit_cell(file.openGroup(nsx::gr_UnitCells + "/" + std::to_string(i)));
            cell_ids.push_back(i);

            // Read the info group and store in metadata
            int n_meta = unit_cell.getNumAttrs();
            for (int j = 0; j < n_meta; ++j) {
                H5::Attribute attr = unit_cell.openAttribute(j);
                H5::DataType typ = attr.getDataType();
                const std::string attr_name = attr.getName();
                if (attr_name == nsx::at_rVec + "_00")
                    attr.read(typ, &rec_00);
                else if (attr_name == nsx::at_rVec + "_01")
                    attr.read(typ, &rec_01);
                else if (attr_name == nsx::at_rVec + "_02")
                    attr.read(typ, &rec_02);
                else if (attr_name == nsx::at_rVec + "_10")
                    attr.read(typ, &rec_10);
                else if (attr_name == nsx::at_rVec + "_11")
                    attr.read(typ, &rec_11);
                else if (attr_name == nsx::at_rVec + "_12")
                    attr.read(typ, &rec_12);
                else if (attr_name == nsx::at_rVec + "_20")
                    attr.read(typ, &rec_20);
                else if (attr_name == nsx::at_rVec + "_21")
                    attr.read(typ, &rec_21);
                else if (attr_name == nsx::at_rVec + "_22")
                    attr.read(typ, &rec_22);
                else if (attr_name == nsx::at_indexingTol)
                    attr.read(typ, &indexing_tolerance);
                else if (attr_name == nsx::at_BravaisLattice)
                    attr.read(typ, bravais);
                else if (attr_name == nsx::at_spacegroup)
                    attr.read(typ, space_group);
                else if (attr_name == nsx::at_z)
                    attr.read(typ, &z);
                else if (attr_name == nsx::at_unitCellName)
                    attr.read(typ, unit_cell_name);
            }
            Eigen::Matrix3d aa = Eigen::Matrix3d::Identity();
            aa << rec_00, rec_01, rec_02, rec_10, rec_11, rec_12, rec_20, rec_21, rec_22;

            UnitCell temp_cell(aa, true);
            temp_cell.setBravaisType(BravaisType(bravais[0]));
            temp_cell.setIndexingTolerance(indexing_tolerance);
            temp_cell.setSpaceGroup(SpaceGroup(space_group));
            temp_cell.setZ(z);
            temp_cell.setLatticeCentring(LatticeCentring(bravais[1]));
            temp_cell.setId(i);
            experiment->addUnitCell(unit_cell_name, temp_cell);
        }

        if (object_num > 0) {
            unsigned int max_val = *std::max_element(cell_ids.begin(), cell_ids.end());
            experiment->setLastUnitCellIndex(max_val + 1);
        }
    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }

    nsxlog(Level::Debug, "Finished importing unit cells from file '", _file_name, "'");
}

void ExperimentImporter::finishLoad() { }

} // namespace nsx
