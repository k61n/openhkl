#pragma once

#include <Eigen/Dense>
#include <map>
#include <vector>

#include "CrystalTypes.h"
#include "DetectorEvent.h"
#include "MillerIndex.h"

namespace nsx {

//! \brief Class to store a library of peak shapes _as covariance matrices n q space_.
//!
class ShapeLibrary {
public:
    ShapeLibrary();
    ~ShapeLibrary();

    //! Add a refernce peak to the library
    void addPeak(sptrPeak3D peak);

    //! Update the fitted covariances
    void updateFit(int num_iterations, double epsilon);

    //! Predict the (detector space) covariance of a given peak
    Eigen::Matrix3d predictCovariance(sptrPeak3D peak);

private:
    std::vector<sptrPeak3D> _strongPeaks;
    Eigen::Matrix3d _covBeam;
    Eigen::Matrix3d _covMosaicity;
    Eigen::Matrix3d _covDetector;
    Eigen::Matrix3d _covShape;
    Eigen::Matrix3d _covBase;
};

} // end namespace nsx
