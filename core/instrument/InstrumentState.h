//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/InstrumentState.h
//! @brief     Defines class InstrumentState
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INSTRUMENT_INSTRUMENTSTATE_H
#define OHKL_CORE_INSTRUMENT_INSTRUMENTSTATE_H

#include <Eigen/Dense>

namespace ohkl {

class Diffractometer;
class DirectVector;
class ReciprocalVector;

/*! \addtogroup python_api
 *  @{*/

/*! \brief Class storing the state of the experiment at a given point in time
 *
 *  State refers to any parameters which might change during the experiment:
 *  sample orientation, sample position, etc. States are initially loaded
 *  as metadata but can also be refined as part of the data treatment.
 *  Note that InstrumentState can be invalid (_valid = false) if interpolation
 *  fails
 */

class InstrumentState {
 public:
    //! default value needed for SWIG (note: nullptr does _not_ work)
    InstrumentState(Diffractometer* diffractometer = nullptr);
    virtual ~InstrumentState() { }

    //! Takes a direct vector in detector coordinates and returns kf in lab coordinates
    ReciprocalVector kfLab(const DirectVector& detector_position) const;

    //! Returns source wavevector k_i
    ReciprocalVector ki() const;

    //! Adjust ki by shifting the direct beam on the detector
    void adjustKi(const DirectVector& detector_position);

    //! Takes direct vector in detector coordinates and returns q in sample coordinates
    ReciprocalVector sampleQ(const DirectVector& detector_position) const;

    //! Returns the gamma angle associated to the given lab space position
    double gamma(const DirectVector& detector_position) const;

    //! Returns the nu angle associated to the given lab space position
    double nu(const DirectVector& detector_position) const;

    //! Returns the 2*theta angle associated to the given lab space position
    double twoTheta(const DirectVector& detector_position) const;

    //! Compute the jacobian of the transformation (x,y) -> k_lab
    Eigen::Matrix3d jacobianK(double px, double py) const;

    //! Set the diffractometer
    void setDiffractometer(Diffractometer* diffractometer);

    //! Returns a pointer to the diffractometer of the state
    Diffractometer* diffractometer();

    //! Returns a const pointer to the diffractometer of the state
    const Diffractometer* diffractometer() const;

    //! Returns whether the InstrumentState is valid
    bool isValid() const;

    //! compute the sample orientation from fixed orientation and offset
    Eigen::Matrix3d detectorOrientationMatrix() const;

    //! Detector orientation as read from metadata
    Eigen::Matrix3d detectorOrientation;

    //! compute the sample orientation from fixed orientation and offset
    Eigen::Matrix3d sampleOrientationMatrix() const;

    //! Sample orientation as read from metadata
    Eigen::Quaterniond sampleOrientation;

    //! Offset to sample orientation, used for parameter refinement
    Eigen::Quaterniond sampleOrientationOffset;

    //! Sample position
    Eigen::Vector3d samplePosition;

    //! Detector offset.
    Eigen::Vector3d detectorPositionOffset;

    //! Incoming beam direction.
    Eigen::RowVector3d ni;

    //! Incoming beam wavelength.
    double wavelength;

    //! True if this state has been refined
    bool refined;

    //! Returns the instrument state as read from the metadata
    static InstrumentState state(Diffractometer* const diffractometer, const std::size_t frame_idx);

    //! Return state as a string
    std::string toString() const;


#ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
#endif

 protected:
    //! Pointer to the diffractometer whose state this object stores.
    //! The actual resource is not owned by this object which just borrows it.
    Diffractometer* _diffractometer;

    //! The state may be invalid (usually because the interpolation has failed)
    bool _valid;
};

using InstrumentStateList = std::vector<InstrumentState>;

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_INSTRUMENT_INSTRUMENTSTATE_H
