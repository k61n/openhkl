//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/data/DataSet.h
//! @brief     Defines class DataSet
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_DATA_DATASET_H
#define OHKL_CORE_DATA_DATASET_H

#include "base/mask/IMask.h"
#include "core/data/DataTypes.h"
#include "core/data/ImageGradient.h"
#include "core/instrument/InstrumentState.h"
#include "core/loader/IDataReader.h"
#include "core/peak/Peak3D.h"
#include "core/raw/DataKeys.h"
//#include "core/loader/TiffDataReader.h"

#include <gsl/gsl_histogram.h>

#include <memory>

namespace ohkl {

class Detector;
class DetectorEvent;
class Diffractometer;
class InstrumentStateSet;
struct RawDataReaderParameters;
struct tif_file_metadata;
struct TiffDataReaderParameters;

enum class BitDepth { u8b = 8, u16b = 16, u32b = 32 };

/*! \addtogroup python_api
 *  @{*/

/*! \brief Class used to manage loading of detector images and metadata from
 *  disk
 *
 *  Note that this class does not contain any actual data, just a few metadata
 *  and a link to the data reader.
 */
class DataSet {
    friend class UnitTest_DataSet;

 public:
    DataSet(const std::string& dataset_name, Diffractometer* diffractometer);
    ~DataSet() = default;

    DataSet() = delete;
    DataSet(const DataSet& other) = delete;
    DataSet& operator=(const DataSet& other) = delete;

    //! Number of detector image frames acquired so far
    std::size_t nFrames() const;
    std::size_t nRows() const; //!< The number of rows in each detector image
    std::size_t nCols() const; //!< The number of columns in each detector image

    void addMask(IMask* mask);
    void removeMask(IMask* mask);
    bool removeMaskByIndex(std::vector<size_t> idx);
    const std::vector<IMask*>& masks() const;

    void maskPeaks(
        std::vector<Peak3D*>& peaks, std::map<Peak3D*, RejectionFlag>& rejection_map,
        double bkg_end = -1.0) const;

    //! Read a single frame
    Eigen::MatrixXi frame(const std::size_t idx) const;
    //! Returns frame after transforming to account for detector gain and baseline
    Eigen::MatrixXd transformedFrame(std::size_t idx) const;
    //! Return per-pixel magnitude of gradient of a given frame
    Eigen::MatrixXd gradientFrame(
        std::size_t idx, GradientKernel kernel, bool realspace = true) const;

    //! Gets the file handle.
    void open();
    //! Close file and release handle
    void close();

    //! Returns the sample-space q vector corresponding to a detector event
    ReciprocalVector computeQ(const DetectorEvent& ev) const;

    //! Returns the data reader used to set this dataset
    const IDataReader* reader() const;
    IDataReader* reader();

    //! Returns the diffractometer associated to this dataset
    const Diffractometer* diffractometer() const;
    //! Returns the diffractometer associated to this dataset
    Diffractometer* diffractometer();
    //! Returns the detector associated to this dataset
    Detector& detector();
    //! Returns the detector associated to this dataset
    const Detector& detector() const;
    std::string name() const;
    void setName(const std::string& name);

    //! Returns a const reference to the MetaData container
    const ohkl::MetaData& metadata() const;
    //! Returns a reference to the MetaData container
    ohkl::MetaData& metadata();

    //! Add a data file for reading data. Reading frames will be done only upon request.
    void addDataFile(const std::string& filename, const std::string& extension);
    //! Set the parameters for the raw-data reader.
    void setRawReaderParameters(const RawDataReaderParameters& params);

    //! Set the parameters for a tif data reader
    void setTiffReaderParameters(const TiffDataReaderParameters& params);

    //! Add a raw file to be read as a single detector image frame. Reading frames will be done only
    //! upon request.
    void addRawFrame(const std::string& rawfilename);

    //! Add a tif file
    void addTiffFrame(const std::string& filename);

    //! Finish reading procedure (must be called before using the data stored in the DataSet).
    void finishRead();

    //! Query the wavelength stored in the metadata
    double wavelength() const;

    //! Query image bit depth stored in metadata
    BitDepth bitDepth() const;

    //! Get the initial instrument states
    void setInstrumentStates(InstrumentStateSet* states);
    //! Return instrument state list
    InstrumentStateList& instrumentStates();

    //! Adjust the direct beam (mainly for scripting purposes)
    void adjustDirectBeam(double x_offset, double y_offset);

    //! Initialise intensity histograms
    void initHistograms(std::size_t nbins);
    //! Generate frame intensity histogram
    void getFrameIntensityHistogram(std::size_t index);
    //! Generate intensity histogram for whole DataSet
    void getIntensityHistogram(std::size_t nbins);
    //! Free histogram memeory
    void clearHistograms();
    //! Maximum per pixel count for whole DataSet
    double maxCount();
    //! getting number of available
    size_t getNumberHistograms() { return _histograms.size(); }

    //! accessing created histograms
    gsl_histogram* getHistogram(int index);
    //! accessing Total histogram
    gsl_histogram* getTotalHistogram();

    //! returns a booleans whether masks have been created or not
    bool hasMasks();
    //! get the number of detector masks
    size_t getNMasks();
    //! remove all detector masks from DataSet
    void removeAllMasks();

    //! Initialise the frame buffer
    void initBuffer(bool bufferAll = true);
    //! Clear the frame buffer
    void clearBuffer();

    //! get tiff file resolutions
    static std::vector<std::string> getTiffResolutions(std::vector<std::string> filenames);

    //! check tiff file resolutions
    static std::string checkTiffResolution(std::vector<std::string> filenames);

    //! return bit depth of tiff files. returns -1 if different bpp values are found
    int getTiffBitDepth(std::vector<std::string> filenames);

 private:
    void setReader(const DataFormat dataformat, const std::string& filename = "");

 private:
    std::string _name = ohkl::kw_datasetDefaultName;
    std::vector<Eigen::MatrixXi> _data;
    std::vector<IMask*> _masks;
    ohkl::MetaData _metadata;
    //! Current data reader (set only once)
    std::shared_ptr<IDataReader> _reader;
    //! Current data format (set only once)
    DataFormat _dataformat = DataFormat::Unknown;
    //! Pointer to the Diffractometer
    Diffractometer* _diffractometer;
    //! Pointer to instrument states
    InstrumentStateSet* _states;

    //! Per-frame histograms of intensities
    std::vector<gsl_histogram*> _histograms;
    //! Intensity histogram for whole DataSet
    gsl_histogram* _total_histogram;

    //! Buffer for image data
    std::vector<std::unique_ptr<Eigen::MatrixXi>> _frame_buffer;
    //! Whether or not the buffer is active
    bool _buffered;

 public:
    //! Data shape (columns, rows, frames)
    std::size_t datashape[3]{0, 0, 0};
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_DATA_DATASET_H
