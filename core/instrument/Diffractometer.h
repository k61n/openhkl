//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Diffractometer.h
//! @brief     Defines class Diffractometer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INSTRUMENT_DIFFRACTOMETER_H
#define NSX_CORE_INSTRUMENT_DIFFRACTOMETER_H

#include "core/detector/Detector.h"
#include "core/instrument/Sample.h"
#include "core/instrument/Source.h"
#include "core/instrument/InstrumentState.h"
#include "core/raw/DataKeys.h"



namespace nsx {

/*! \addtogroup python_api
 *  @{*/

/*! \brief diffractometer setup, consisting of a sample, source, and detector.
 *
 */

class Diffractometer {
 public:
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

 public:
    //! Vector of sample states
    std::vector<std::vector<double>> sampleStates;

    //! Vector of detector states
    std::vector<std::vector<double>> detectorStates;

 protected:
    Diffractometer();

    //! Constructs a diffractometer with a given name
    Diffractometer(const std::string& name);

    //! Name of the diffractometer
    std::string _name = nsx::kw_diffractometerDefaultName;

    //! Pointer to detector
    std::unique_ptr<Detector> _detector;

    //! The sample
    Sample _sample;

    //! The neutron incoming beam
    Source _source;
};

/*! @}*/
} // namespace nsx

#endif // NSX_CORE_INSTRUMENT_DIFFRACTOMETER_H
