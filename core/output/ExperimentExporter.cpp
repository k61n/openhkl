//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/PeakExporter.cpp
//! @brief     Class for peak export to file
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************


#include "core/output/ExperimentExporter.h"



#include "base/parser/BloscFilter.h"
#include "core/peak/Peak3D.h"
#include "tables/crystal/UnitCell.h"
#include "base/utils/Units.h" // deg
#include "core/detector/Detector.h"
#include "core/detector/DetectorEvent.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Diffractometer.h"
#include "core/instrument/Monochromator.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "tables/crystal/UnitCell.h"

#include <Eigen/Dense>
#include <vector>
#include <iostream>
namespace nsx {

bool ExperimentExporter::createFile(std::string path)
{
    try{

        std::cout<<"Creating the file ... "<<std::endl;

        H5::H5File* file = new H5::H5File(path.c_str(), H5F_ACC_TRUNC);
        _file_name = path;
        delete file;
        
    } catch (...){
        return false;
    }

    return true;
}

bool ExperimentExporter::writeData(
    const std::map<std::string, DataSet*> data)
{
    try{
        
        H5::H5File* file = new H5::H5File(_file_name.c_str(), H5F_ACC_RDWR);
        H5::Group* data_collections = new H5::Group(file->createGroup("/DataCollections"));
        H5::Group* data_collection;
        H5::Group* detector_group;
        H5::Group* sample_group;
        H5::Group* info_group;
        H5::Group* meta_data_group;

        for (
            std::map<std::string,DataSet*>::const_iterator it = data.begin(); 
            it != data.end(); ++it) {

            //Write the data
            std::string data_name = it->first;
            DataSet* data_item = it->second;

            std::string name;
            std::string ext;

            size_t sep = data_name.find_last_of("\\/");
            if (sep != std::string::npos)
                data_name = data_name.substr(sep + 1, data_name.size() - sep - 1);

            size_t dot = data_name.find_last_of(".");
            if (dot != std::string::npos){
                name = data_name.substr(0, dot);
                ext  = data_name.substr(dot, data_name.size() - dot);
            }else{
                name = data_name;
                ext  = "";
            }

            data_collection = new H5::Group(
                file->createGroup(std::string("/DataCollections/"+name)));
            
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
                throw std::runtime_error("Problem registering BLOSC filter in HDF5 library");

            // caught by valgrind memcheck
            free(version);
            version = nullptr;
            free(date);
            date = nullptr;
            plist.setFilter(FILTER_BLOSC, H5Z_FLAG_OPTIONAL, 7, cd_values);

            H5::DataSpace space(3, dims, nullptr);
            H5::DataSet dset(
                file->createDataSet(std::string("/DataCollections/"+name+"/"+name), H5::PredType::NATIVE_INT32, space, plist));

            hsize_t offset[3];
            offset[0] = 0;
            offset[1] = 0;
            offset[2] = 0;

            H5::DataSpace memspace(3, count, nullptr);
            using IntMatrix = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
            for (offset[0] = 0; offset[0] < data_item->nFrames(); offset[0] += count[0]) {
                space.selectHyperslab(H5S_SELECT_SET, count, offset, nullptr, nullptr);
                // HDF5 requires row-major storage, so copy frame into a row-major matrix
                IntMatrix current_frame(data_item->frame(offset[0]));
                dset.write(current_frame.data(), H5::PredType::NATIVE_INT32, memspace, space);
            }

            delete data_collection;
            blosc_destroy();

            // Write detector states
            detector_group = new H5::Group(
                file->createGroup(std::string("/DataCollections/"+name+"/Detector")));

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
                H5::DataSet detector_scan(
                    file->createDataSet(
                        std::string("/DataCollections/"+name+"/Detector/"+axis.name()),
                        H5::PredType::NATIVE_DOUBLE, scanSpace));
                detector_scan.write(&values(0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
            }
            delete detector_group;

            // Write sample states
            sample_group = new H5::Group(
                file->createGroup(std::string("/DataCollections/"+name+"/Sample")));

            const auto& sampleStates = data_item->reader()->sampleStates();
            const auto& sample_gonio = data_item->reader()->diffractometer()->sample().gonio();
            size_t n_sample_gonio_axes = sample_gonio.nAxes();

            for (size_t i = 0; i < n_sample_gonio_axes; ++i) {
                const auto& axis = sample_gonio.axis(i);
                Eigen::VectorXd values(data_item->nFrames());
                for (size_t j = 0; j < data_item->nFrames(); ++j)
                    values(j) = sampleStates[j][i] / deg;
                H5::DataSet sample_scan(
                    file->createDataSet(
                        std::string("/DataCollections/"+name+"/Sample/"+axis.name()),
                        H5::PredType::NATIVE_DOUBLE, scanSpace));
                sample_scan.write(&values(0), H5::PredType::NATIVE_DOUBLE, scanSpace, scanSpace);
            }
            delete sample_group;
            
            const auto& map = data_item->reader()->metadata().map();

            // Write all string metadata into the "Info" group
            info_group = new H5::Group(
                file->createGroup(std::string("/DataCollections/"+name+"/Info")));

            H5::DataSpace metaSpace(H5S_SCALAR);
            H5::StrType str80(H5::PredType::C_S1, 80);
            std::string info;

            for (const auto& item : map) {
                std::string info;

                try {
                    info = item.second.as<std::string>();
                    H5::Attribute intAtt(info_group->createAttribute(item.first, str80, metaSpace));
                    intAtt.write(str80, info);
                } catch (...) {
                    // shouldn't there be some error handling here?
                }
            }
            delete info_group;

            // Write all other metadata (int and double) into the "Experiment" Group
            meta_data_group = new H5::Group(
                file->createGroup(std::string("/DataCollections/"+name+"/Meta")));

            for (const auto& item : map) {
                int value;

                try {
                    value = item.second.as<int>();
                    H5::Attribute intAtt(
                        meta_data_group->createAttribute(item.first, H5::PredType::NATIVE_INT32, metaSpace));
                    intAtt.write(H5::PredType::NATIVE_INT, &value);
                } catch (...) {
                    try {
                        double d_value;
                        d_value = item.second.as<double>();
                        H5::Attribute intAtt(meta_data_group->createAttribute(
                            item.first, H5::PredType::NATIVE_DOUBLE, metaSpace));
                        intAtt.write(H5::PredType::NATIVE_DOUBLE, &d_value);
                    } catch (...) {
                        // shouldn't there be some error handling here?
                    }
                }

            }
            delete meta_data_group;
        }

        delete data_collections;
        delete file;

    } catch (...){
        return false;
    }
    
    return true;
}


bool ExperimentExporter::writeInstrument(const Diffractometer* diffractometer)
{
    try{

    } catch (...){
        return false;
    }
    
    return true;
}

bool ExperimentExporter::writePeaks(
    const std::map<std::string, PeakCollection*> peakCollections
)
{
    try{

        H5::H5File* file = new H5::H5File(_file_name.c_str(), H5F_ACC_RDWR);
        H5::Group* peak_collections = new H5::Group(file->createGroup("/PeakCollections"));
        H5::Group* peak_collection;

        //initialise doubles
        double* peak_end;
        double* bkg_begin;
        double* bkg_end;
        double* scale;
        double* transimition;
        double* intensity;
        double* sigma;

        //initialise the booleans
        bool* selected;
        bool* masked;
        bool* predicted;

        for (
            std::map<std::string,PeakCollection*>::const_iterator it = peakCollections.begin(); 
            it != peakCollections.end(); ++it){

            //Write the data
            std::string collection_name = it->first;
            PeakCollection* collection_item = it->second;

            peak_collection = new H5::Group(
                file->createGroup(std::string("/PeakCollections/"+collection_name)));

            //initialise doubles
            peak_end = new double[collection_item->numberOfPeaks()];
            bkg_begin = new double[collection_item->numberOfPeaks()];
            bkg_end = new double[collection_item->numberOfPeaks()];
            scale = new double[collection_item->numberOfPeaks()];
            transimition = new double[collection_item->numberOfPeaks()];
            intensity = new double[collection_item->numberOfPeaks()];
            sigma = new double[collection_item->numberOfPeaks()];

            Eigen::Matrix<double,Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> center(collection_item->numberOfPeaks(), 3);
            Eigen::Matrix<double,Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> metric(3*collection_item->numberOfPeaks(), 3);

            center.setZero(collection_item->numberOfPeaks(), 3);
            metric.setZero(3*collection_item->numberOfPeaks(), 3);

            //initialise the booleans
            selected = new bool[collection_item->numberOfPeaks()];
            masked = new bool[collection_item->numberOfPeaks()];
            predicted = new bool[collection_item->numberOfPeaks()];

            //initialize the datanames
            std::vector<const char*> data_names;
            std::vector<const char*> unit_cells;

            std::string name;
            std::string ext;

            for (int i = 0; i < collection_item->numberOfPeaks(); ++i) {
                
                nsx::Peak3D* peak = collection_item->getPeak(i);

                //set the values
                peak_end[i] = peak->peakEnd();
                bkg_begin[i] = peak->bkgBegin();
                bkg_end[i] = peak->bkgEnd();
                scale[i] = peak->scale();
                transimition[i] = peak->transmission();
                intensity[i] = peak->rawIntensity().value();
                sigma[i] = peak->rawIntensity().sigma();

                selected[i] = peak->selected();
                masked[i] = peak->masked();
                predicted[i] = peak->predicted();

                std::string data_name = peak->data()->filename();
                size_t sep = data_name.find_last_of("\\/");
                if (sep != std::string::npos)
                    data_name = data_name.substr(sep + 1, data_name.size() - sep - 1);

                size_t dot = data_name.find_last_of(".");
                if (dot != std::string::npos){
                    name = data_name.substr(0, dot);
                    ext  = data_name.substr(dot, data_name.size() - dot);
                }else{
                    name = data_name;
                    ext  = "";
                }

                data_names.push_back(name.c_str());

                sptrUnitCell unit_cell = peak->unitCell();
                if (unit_cell){
                    unit_cells.push_back(unit_cell->name().c_str());
                }else{
                    std::string temp = "";
                    unit_cells.push_back(temp.c_str());
                }
                
                center.block( i, 0, 1, 3) = peak->shape().center();
                metric.block( i * 3, 0, 3, 3) = peak->shape().metric();

            }

            hsize_t num_peaks[1] = {collection_item->numberOfPeaks()};
            H5::DataSpace peak_space(1, num_peaks);
            hsize_t center_peaks_h[2] = {collection_item->numberOfPeaks(), 3};
            H5::DataSpace center_space(2, center_peaks_h);
            hsize_t metric_peaks_h[2] = {collection_item->numberOfPeaks()*3, 3};
            H5::DataSpace metric_space(2, metric_peaks_h);

            H5::DataSet peak_end_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/PeakEnd"),
                    H5::PredType::NATIVE_DOUBLE, peak_space));
            peak_end_H5.write(
                peak_end, H5::PredType::NATIVE_DOUBLE, 
                peak_space, peak_space);

            H5::DataSet bkg_begin_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/BkgBegin"),
                    H5::PredType::NATIVE_DOUBLE, peak_space));
            bkg_begin_H5.write(
                bkg_begin, H5::PredType::NATIVE_DOUBLE, 
                peak_space, peak_space);

            H5::DataSet bkg_end_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/BkgEnd"),
                    H5::PredType::NATIVE_DOUBLE, peak_space));
            bkg_end_H5.write(
                bkg_end, H5::PredType::NATIVE_DOUBLE, 
                peak_space, peak_space);

            H5::DataSet scale_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/Scale"),
                    H5::PredType::NATIVE_DOUBLE, peak_space));
            scale_H5.write(
                scale, H5::PredType::NATIVE_DOUBLE, 
                peak_space, peak_space);

            H5::DataSet transimition_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/Transmission"),
                    H5::PredType::NATIVE_DOUBLE, peak_space));
            transimition_H5.write(
                transimition, H5::PredType::NATIVE_DOUBLE, 
                peak_space, peak_space);

            H5::DataSet intensity_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/Intensity"),
                    H5::PredType::NATIVE_DOUBLE, peak_space));
            intensity_H5.write(
                intensity, H5::PredType::NATIVE_DOUBLE, 
                peak_space, peak_space);

            H5::DataSet sigma_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/Sigma"),
                    H5::PredType::NATIVE_DOUBLE, peak_space));
            sigma_H5.write(
                sigma, H5::PredType::NATIVE_DOUBLE, 
                peak_space, peak_space);

            H5::DataSet center_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/Center"),
                    H5::PredType::NATIVE_DOUBLE, center_space));
            center_H5.write(
                center.data(), H5::PredType::NATIVE_DOUBLE, 
                center_space, center_space);

            H5::DataSet metric_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/Metric"),
                    H5::PredType::NATIVE_DOUBLE, metric_space));
            metric_H5.write(
                metric.data(), H5::PredType::NATIVE_DOUBLE, 
                metric_space, metric_space);

            H5::DataSet selected_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/Selected"),
                    H5::PredType::NATIVE_HBOOL, peak_space));
            selected_H5.write(
                selected, H5::PredType::NATIVE_HBOOL, 
                peak_space, peak_space);

            H5::DataSet masked_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/Masked"),
                    H5::PredType::NATIVE_HBOOL, peak_space));
            masked_H5.write(
                masked, H5::PredType::NATIVE_HBOOL, 
                peak_space, peak_space);

            H5::DataSet predicted_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/Predicted"),
                    H5::PredType::NATIVE_HBOOL, peak_space));
            predicted_H5.write(
                predicted, H5::PredType::NATIVE_HBOOL, 
                peak_space, peak_space);

            H5::StrType datatype(H5::PredType::C_S1, H5T_VARIABLE); 
            H5::DataSet data_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/DataNames"),
                    datatype, peak_space));
            data_H5.write(
                data_names.data(), datatype, 
                peak_space, peak_space);

            H5::DataSet unit_cell_H5(
                file->createDataSet(
                    std::string("/PeakCollections/"+collection_name+"/UnitCells"),
                    datatype, peak_space));
            unit_cell_H5.write(
                unit_cells.data(), datatype, 
                peak_space, peak_space);

            // Write all other metadata (int and double) into the "Experiment" Group
            H5::Group meta_peak_group(
                file->createGroup(std::string("/PeakCollections/"+collection_name+"/Meta")));

            std::map<std::string, float>* map = collection_item->meta();
            H5::DataSpace metaSpace(H5S_SCALAR);
            H5::StrType str80(H5::PredType::C_S1, 80);

            for (const auto& item : *map) {
                int value;

                try {
                    value = item.second;
                    H5::Attribute intAtt(
                        meta_peak_group.createAttribute(item.first, H5::PredType::NATIVE_INT32, metaSpace));
                    intAtt.write(H5::PredType::NATIVE_INT32, &value);
                } catch (...) {
                }

            }
            //initialise doubles
            delete[] peak_end;
            delete[] bkg_begin;
            delete[] bkg_end;
            delete[] scale;
            delete[] transimition;

            //initialise the booleans
            delete[] selected;
            delete[] masked;
            delete[] predicted;
            
            delete peak_collection;

        }

        delete peak_collections;
        delete file;

    } catch (...){
        return false;
    }
    
    return true;
}

bool ExperimentExporter::writeUnitCells()
{
    try{

    } catch (...){
        return false;
    }
    
    return true;
}

bool ExperimentExporter::finishWrite()
{
    try{
        // _file->close();
        
    } catch (...){
        return false;
    }
    
    return true;
}

} //namespace nsx