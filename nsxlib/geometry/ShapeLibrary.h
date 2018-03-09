#pragma once

#include <Eigen/Dense>
#include <map>
#include <vector>

#include "CrystalTypes.h"
#include "DetectorEvent.h"
#include "MillerIndex.h"

namespace nsx {

struct FitData;

//! \brief Class to store a library of peak shapes _as covariance matrices n q space_.
//!
class ShapeLibrary {
public:
    ShapeLibrary();
    ~ShapeLibrary();

    //! Add a refernce peak to the library
    bool addPeak(sptrPeak3D peak);

    //! Update the fitted covariances
    void updateFit(int num_iterations);

    //! Predict the (detector space) covariance of a given peak
    Eigen::Matrix3d predictCovariance(sptrPeak3D peak) const;

    //! Predict the (detector space) covariance given the fit data
    Eigen::Matrix3d predictCovariance(const FitData& data) const;

    //! Return mean Pearson coefficient to measure quality of fit
    double meanPearson() const;

private:
    std::vector<sptrPeak3D> _strongPeaks;
    Eigen::Matrix3d _covBeam;
    Eigen::Matrix3d _covMosaicity;
    Eigen::Matrix3d _covDetector;
    Eigen::Matrix3d _covShape;
    Eigen::Matrix3d _covBase;
};

} // end namespace nsx
