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
#include "core/loader/ExperimentDataReader.h"
#include "core/peak/Intensity.h"
#include "core/peak/Peak3D.h"
#include "core/raw/IDataReader.h"
#include <Eigen/Dense>

#include <string>
#include "H5Cpp.h"

namespace nsx {

void ExperimentImporter::setFilePath(const std::string path, Experiment* const experiment)
{
    try {
        _file_name = path;
        H5::H5File file(_file_name.c_str(), H5F_ACC_RDONLY);
        if (file.attrExists("name")) {
            const H5::Attribute attr = file.openAttribute("name");
            const H5::DataType attr_type = attr.getDataType();
            std::string value;
            attr.read(attr_type, value);
            experiment->setName(value);
        }

        if (file.attrExists("diffractometer")) {
            const H5::Attribute attr = file.openAttribute("diffractometer");
            const H5::DataType attr_type = attr.getDataType();
            std::string value;
            attr.read(attr_type, value);
            experiment->setDiffractometer(value);
        }

    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }
}

void ExperimentImporter::loadData(Experiment* experiment)
{
    try {
        H5::H5File file(_file_name.c_str(), H5F_ACC_RDONLY);
        H5::Group data_collections(file.openGroup("/DataCollections"));

        hsize_t object_num = data_collections.getNumObjs();
        for (int i = 0; i < object_num; ++i) {
            auto reader = std::make_unique<nsx::ExperimentDataReader>(
                _file_name, data_collections.getObjnameByIdx(i), experiment->getDiffractometer());
            nsx::sptrDataSet data{new nsx::DataSet{std::move(reader)}};

            const std::string collection_name = data_collections.getObjnameByIdx(i);

            experiment->addData(data, collection_name);
        }
    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }
}

void ExperimentImporter::loadPeaks(Experiment* experiment)
{
    using Eigen_VecXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::RowMajor>;
    using Eigen_VecXint = Eigen::Matrix<int, Eigen::Dynamic, Eigen::RowMajor>;
    using Eigen_VecXbool = Eigen::Matrix<bool, Eigen::Dynamic, Eigen::RowMajor>;

    try {
        H5::H5File file(_file_name.c_str(), H5F_ACC_RDONLY);
        H5::Group peak_collections(file.openGroup("/PeakCollections"));

        hsize_t object_num = peak_collections.getNumObjs();
        for (int i = 0; i < object_num; ++i) {
            const std::string collection_name = peak_collections.getObjnameByIdx(i);
            const std::string collection_key = "/PeakCollections/" + collection_name;
            H5::Group peak_collection(file.openGroup(collection_key));

            H5::Group peak_collection_meta(
                file.openGroup(collection_key + "/Meta"));
            // Read the info group and store in metadata
            int n_peaks = 0;
            int type = 0;

            if (peak_collection_meta.attrExists("num_peaks")) {
                const H5::Attribute attr = peak_collection_meta.openAttribute("num_peaks");
                const H5::DataType attr_type = attr.getDataType();
                attr.read(attr_type, &n_peaks);
            }

            if (peak_collection_meta.attrExists("Type")) {
                const H5::Attribute attr = peak_collection_meta.openAttribute("Type");
                const H5::DataType attr_type = attr.getDataType();
                attr.read(attr_type, &type);
            }

            nsxlog(Level::Debug, "ExperimentImporter::loadPeaks: found", n_peaks, "to import");
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

            std::map<std::string, Eigen_VecXd*> double_keys {
                 {"BkgBegin", &bkg_begin},
                 {"BkgEnd", &bkg_end},
                 {"PeakEnd", &peak_end},
                 {"Scale", &scale},
                 {"Transmission", &transmission},
                 {"Intensity", &intensity},
                 {"Sigma", &sigma},
                 {"BkgIntensity", &mean_bkg_val},
                 {"BkgSigma", &mean_bkg_sig}
            };

            std::map<std::string, Eigen_VecXint*> int_keys {{"Rejection", &rejection_flag}};

            Eigen_VecXbool predicted(n_peaks);
            Eigen_VecXbool masked(n_peaks);
            Eigen_VecXbool selected(n_peaks);

            std::map<std::string, Eigen_VecXbool*> bool_keys
                {
                 {"Predicted", &predicted},
                 {"Masked", &masked},
                 {"Selected", &selected}
                };

            Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> center(
                n_peaks, 3);
            Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> metric(
                3 * n_peaks, 3);

            std::vector<std::string> data_names;
            std::vector<std::string> unit_cells;

            nsxlog(Level::Debug, "Loading doubles");
            // Load all doubles
            for (std::map<std::string, Eigen_VecXd*>::iterator it = double_keys.begin();
                 it != double_keys.end(); it++) {
                H5::DataSet data_set = peak_collection.openDataSet(it->first);
                H5::DataSpace space(data_set.getSpace());
                data_set.read(it->second->data(), H5::PredType::NATIVE_DOUBLE, space, space);
            }

            nsxlog(Level::Debug, "Loading integers");
            // Load all ints
            for (std::map<std::string, Eigen_VecXint*>::iterator it = int_keys.begin();
                 it != int_keys.end(); it++) {
                H5::DataSet data_set = peak_collection.openDataSet(it->first);
                H5::DataSpace space(data_set.getSpace());
                data_set.read(it->second->data(), H5::PredType::NATIVE_INT, space, space);
            }

            nsxlog(Level::Debug, "Loading booleans");
            // Load all booleans
            for (std::map<std::string, Eigen_VecXbool*>::iterator it = bool_keys.begin();
                 it != bool_keys.end(); it++) {
                H5::DataSet data_set = peak_collection.openDataSet(it->first);
                H5::DataSpace space(data_set.getSpace());
                Eigen_VecXbool* item = it->second;
                data_set.read(item->data(), H5::PredType::NATIVE_HBOOL, space, space);
            }

            nsxlog(Level::Debug, "Loading centers");
            // Load the centers
            {
                H5::DataSet data_set = peak_collection.openDataSet("Center");
                H5::DataSpace space(data_set.getSpace());
                data_set.read(center.data(), H5::PredType::NATIVE_DOUBLE, space, space);
            }

            nsxlog(Level::Debug, "Loading metric");
            // Load the metrics
            {
                H5::DataSet data_set = peak_collection.openDataSet("Metric");
                H5::DataSpace space(data_set.getSpace());
                data_set.read(metric.data(), H5::PredType::NATIVE_DOUBLE, space, space);
            }

            nsxlog(Level::Debug, "Loading DataSet names");
            // Load the data_names
            {
                H5::DataSet data_set = peak_collection.openDataSet("DataNames");
                H5::DataType data_type = data_set.getDataType();
                H5::DataSpace space(data_set.getSpace());

                hsize_t data_dims_out[2];
                space.getSimpleExtentDims(data_dims_out, nullptr);

                char** char_data_names = new char*[data_dims_out[0]];
                data_set.read((void*)char_data_names, data_type);

                for (int ii = 0; ii < data_dims_out[0]; ++ii) {
                    std::string text;
                    for (int jj = 0; jj < strlen(char_data_names[ii]); ++jj)
                        text.append(std::string(1, char_data_names[ii][jj]));
                    data_names.push_back(text);
                }

                delete[] char_data_names;
            }

            nsxlog(Level::Debug, "Loading UnitCell names");
            // Load the unit cell strings
            {
                H5::DataSet uc_data_set = peak_collection.openDataSet("UnitCells");
                H5::DataType uc_data_type = uc_data_set.getDataType();
                H5::DataSpace uc_space = uc_data_set.getSpace();

                hsize_t uc_dims_out[2];
                uc_space.getSimpleExtentDims(uc_dims_out, nullptr);

                char** char_unit_cells = new char*[uc_dims_out[0]];
                uc_data_set.read((void*)char_unit_cells, uc_data_type);

                for (int ii = 0; ii < uc_dims_out[0]; ++ii) {
                    std::string text;
                    for (int jj = 0; jj < strlen(char_unit_cells[ii]); ++jj)
                        text.append(std::string(1, char_unit_cells[ii][jj]));
                    unit_cells.push_back(text);
                }

                delete[] char_unit_cells;
            }

            nsxlog(Level::Debug, "Finished reading data from file");
            nsxlog(Level::Debug, "Creating the vector of peaks");
            std::vector<nsx::Peak3D*> peaks;

            Eigen::Vector3d local_center;
            Eigen::Matrix3d local_metric;

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

                const UnitCell* unit_cell_pointer = experiment->getUnitCell(unit_cells[k]);
                peak->setUnitCell(unit_cell_pointer);

                peaks.push_back(peak);
            }
            nsxlog(Level::Debug, "Finished creating the vector of peaks");

            listtype collection_type = static_cast<listtype>(type);
            experiment->addPeakCollection(collection_name, collection_type, peaks);

            nsxlog(Level::Debug, "Created the peak collection");
        }
    } catch (H5::Exception& e) {
        throw std::runtime_error{e.getDetailMsg()};
    }
}

void ExperimentImporter::loadUnitCells(Experiment* experiment)
{
    try {
        H5::H5File file(_file_name.c_str(), H5F_ACC_RDONLY);
        H5::Group unit_cells(file.openGroup("/UnitCells"));

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

            std::string cell_name = unit_cells.getObjnameByIdx(i);
            H5::Group unit_cell(file.openGroup("/UnitCells/" + cell_name));

            // Read the info group and store in metadata
            int n_meta = unit_cell.getNumAttrs();
            for (int j = 0; j < n_meta; ++j) {
                H5::Attribute attr = unit_cell.openAttribute(j);
                H5::DataType typ = attr.getDataType();

                if (attr.getName() == "rec_00")
                    attr.read(typ, &rec_00);
                if (attr.getName() == "rec_01")
                    attr.read(typ, &rec_01);
                if (attr.getName() == "rec_02")
                    attr.read(typ, &rec_02);
                if (attr.getName() == "rec_10")
                    attr.read(typ, &rec_10);
                if (attr.getName() == "rec_11")
                    attr.read(typ, &rec_11);
                if (attr.getName() == "rec_12")
                    attr.read(typ, &rec_12);
                if (attr.getName() == "rec_20")
                    attr.read(typ, &rec_20);
                if (attr.getName() == "rec_21")
                    attr.read(typ, &rec_21);
                if (attr.getName() == "rec_22")
                    attr.read(typ, &rec_22);

                if (attr.getName() == "indexing_tolerance")
                    attr.read(typ, &indexing_tolerance);
                if (attr.getName() == "bravais")
                    attr.read(typ, bravais);
                if (attr.getName() == "space_group")
                    attr.read(typ, space_group);
                if (attr.getName() == "z")
                    attr.read(typ, &z);
            }

            Eigen::Matrix3d aa = Eigen::Matrix3d::Identity();
            aa << rec_00, rec_01, rec_02, rec_10, rec_11, rec_12, rec_20, rec_21, rec_22;

            UnitCell temp_cell(aa, true);
            temp_cell.setName(cell_name);
            temp_cell.setBravaisType(BravaisType(bravais[0]));
            temp_cell.setIndexingTolerance(indexing_tolerance);
            temp_cell.setSpaceGroup(SpaceGroup(space_group));
            temp_cell.setZ(z);
            temp_cell.setLatticeCentring(LatticeCentring(bravais[1]));
            experiment->addUnitCell(cell_name, temp_cell);
        }
    } catch (H5::Exception& e) {
        std::string what = e.getDetailMsg();
        throw std::runtime_error(what);
    }
}

void ExperimentImporter::finishLoad() { }

} // namespace nsx
