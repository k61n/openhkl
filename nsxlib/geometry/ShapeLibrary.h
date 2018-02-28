#pragma once

#include <Eigen/Dense>
#include <map>
#include <vector>

#include "DetectorEvent.h"
#include "FitProfile.h"
#include "MillerIndex.h"

namespace nsx {

//! \brief Class to store a library of peak shapes _as covariance matrices n q space_.
//!
class ShapeLibrary {
public:
    ShapeLibrary();
    ~ShapeLibrary();

    //! Add a shape to the library.
    void addShape(const DetectorEvent& ev, const FitProfile& profile);

    //! Set the default shape
    void setDefaultShape(const FitProfile& profile);

    //! Get average shapes within the given region of the detector:
    //! Find entries in library such that the pixel coordinates differ from event by at most
    //! _radius_ and such that the difference in frame numbers is at most _nframes_.
    FitProfile average(const DetectorEvent& ev, double radius, double nframes) const;

    //! Return the average of all shapes
    FitProfile meanShape() const;

private:
    std::vector<std::pair<DetectorEvent, FitProfile>> _shapes;
    FitProfile _defaultShape;
};

} // end namespace nsx
