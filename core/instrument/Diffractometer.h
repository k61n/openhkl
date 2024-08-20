//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/Diffractometer.h
//! @brief     Defines class Diffractometer
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INSTRUMENT_DIFFRACTOMETER_H
#define OHKL_CORE_INSTRUMENT_DIFFRACTOMETER_H

#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/loader/IDataReader.h"
#include "core/raw/DataKeys.h"

namespace ohkl {

class Detector;
class InstrumentState;

using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

/*! \addtogroup python_api
 *  @{*/

/*! \brief#include "core/loader/IDataReader.h"
 diffractometer setup, consisting of a sample, source, and detector.
 *
 */

class Diffractometer {
 public:
    Diffractometer() = delete;
    static Diffractometer* create(const std::string& name);

    Diffractometer(const YAML::Node& node);
    virtual ~Diffractometer();

    //! Returns the name of this diffractometer
    const std::string& name() const;

    //! Sets the name of the diffractometer
    void setName(const std::string& name);

    //! Returns a pointer to the detector of this diffractometer
    Detector* detector();

    //! Returns const pointer to the detector of this diffractometer
    const Detector* detector() const;

    //! Returns the instrument state
    InstrumentState instrumentState(const std::size_t frame_idx);

#ifndef SWIG
    //! Sets the detector of this diffractometer
    void setDetector(std::unique_ptr<Detector> detector);
#endif

    //! Returns the non-const reference to the sample of this diffractometer
    Sample& sample();

    //! Returns the const reference to the sample of this diffractometer
    const Sample& sample() const;

    //! Sets the sample of this diffractometer
    void setSample(const Sample& sample);

    //! Returns the non-const reference to the source of this diffractometer
    Source& source();

    //! Returns the const reference to the source of this diffractometer
    const Source& source() const;

    //! Sets the source of this diffractometer
    void setSource(const Source& source);

    //! Add angles for a sample state
    void addSampleAngles(std::size_t frame_idx, const DataReaderParameters& params);

    //! Add angles for a given sample state
    void addSampleAngles(const std::vector<double>& angles);

    //! Add angles for a detector state
    void addDetectorAngles(const DataReaderParameters& params);

    //! Add angles for a detector state
    void addDetectorAngles(const std::vector<double>& angles);

    //! Get the sample angles
    const std::vector<std::vector<double>>& sampleAngles() const { return _sample_angles; };

    //! Get the detector angles
    const std::vector<std::vector<double>>& detectorAngles() const { return _detector_angles; };

    //! Set the sample angles
    void setSampleAngles(const RowMatrixXd& mat, std::size_t nframes);

    //! Set the detector angles
    void setDetectorAngles(const RowMatrixXd& mat, std::size_t nframes);

 protected:
    //! Constructs a diffractometer with a given name
    Diffractometer(const std::string& name);

    //! Name of the diffractometer
    std::string _name;

    //! Pointer to detector
    std::unique_ptr<Detector> _detector;

    //! The sample
    Sample _sample;

    //! The neutron incoming beam
    Source _source;

    //! Vector of sample states
    std::vector<std::vector<double>> _sample_angles;

    //! Vector of detector states
    std::vector<std::vector<double>> _detector_angles;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_INSTRUMENT_DIFFRACTOMETER_H
