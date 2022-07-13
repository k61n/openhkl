//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/detector/Detector.h
//! @brief     Defines class Detector
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_DETECTOR_DETECTOR_H
#define NSX_CORE_DETECTOR_DETECTOR_H

#include "core/gonio/Component.h" // inheriting from

#include "base/geometry/ReciprocalVector.h"
#include "core/detector/DetectorEvent.h"

namespace ohkl {

//! Pure virtual base class for detectors of different geometry.

class Detector : public Component {
 public:
    //! Static constructor of a Detector from a property tree node
    static Detector* create(const YAML::Node& node);

    Detector();
    Detector(const std::string& name);
    Detector(const YAML::Node& node);
    virtual ~Detector() = 0;

    virtual Detector* clone() const = 0;

    //! Returns the sample to detector distance (meters)
    double distance() const;
    //! Sets sample to detector distance
    void setDistance(double d);

    //! Returns the number of columns of the detector
    unsigned int nCols() const;
    //! Sets the number of columns of the detector
    void setNCols(unsigned int cols);
    //! Returns the number of rows of the detector
    unsigned int nRows() const;
    //! Sets the number of rows
    void setNRows(unsigned int rows);

    //!  Get the minimum row index
    int minRow() const;
    //!  Get the maximum row index
    int maxRow() const;
    //!  Get the minimum col index
    int minCol() const;
    //!  Get the maximum col index
    int maxCol() const;

    //! Returns true if a given pixel is inside the detector
    bool hasPixel(double px, double py) const;

    //! Returns the height of the detector (meters)
    double height() const;
    //! Sets the height of the detector (meters)
    virtual void setHeight(double height) = 0;

    //! Returns the width of the detector (meters)
    double width() const;
    //! Sets the width of the detector (meters)
    virtual void setWidth(double width) = 0;

    //! Returns the height in angular units of the detector (radians)
    double angularHeight() const;
    //! Sets the height in angular units of the detector (radians)
    virtual void setAngularHeight(double angle) = 0;

    //! Returns the width in angular units of the detector (radians)
    double angularWidth() const;
    //! Sets the height in angular units of the detector (radians)
    virtual void setAngularWidth(double angle) = 0;

    //! Returns the detector event (pixel x, pixel y, time of flight) associated
    //! with a given kf. Returns with _negative_ tof if no such event is possible.
    virtual DetectorEvent constructEvent(
        const DirectVector& from, const ReciprocalVector& kf, const double frame) const = 0;

    //! Returns the position of a given pixel in detector space. This takes into
    //! account the detector motions in detector space.
    virtual DirectVector pixelPosition(double x, double y) const = 0;
    //! Returns the mean detector pixel height
    double pixelHeight() const;
    //! Returns the mean detector pixel width
    double pixelWidth() const;

    //! Returns the Jacobian matrix of the transformation (px,py) -> (x,y,z) from
    //! pixel coordinates to lab coordinates. The first and second columns are the
    //! derivatives of (x,y,z) with respect to px and py. The third column is
    //! identically zero and is kept only for convenience.
    virtual Eigen::Matrix3d jacobian(double x, double y) const = 0;

    //! Returns the detector baseline. Measured count = gain * (neutron count) + baseline
    double baseline() const;
    //! Returns the detector gain. Measured count = gain * (neutron count) + baseline
    double gain() const;

    //! Override the baseline from the .yml2c file
    void setBaseline(double baseline);
    //! Override the gain from the .yml2c file
    void setGain(double gain);

 protected:
    //! Detector height
    double _height;
    //! Detector width
    double _width;
    //! Detector angular height
    double _angularHeight;
    //! Detector angular width
    double _angularWidth;
    //! Number of rows of pixels
    unsigned int _nRows;
    //! Number of columns of pixels
    unsigned int _nCols;
    //! Minimum row number (y origin)
    double _minRow;
    //! Minimum column number (x origin)
    double _minCol;
    //! Distance from origin to detector
    double _distance;

 private:
    //! Detector baseline. Default is 0.0
    double _baseline;
    //! Detector gain. Default is 1.0
    double _gain;
};

} // namespace ohkl

#endif // NSX_CORE_DETECTOR_DETECTOR_H
