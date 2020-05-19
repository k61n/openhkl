//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/ShapeLibrary.h
//! @brief     Defines classes PeakInterpolation, ShapeLibrary
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_SHAPE_SHAPELIBRARY_H
#define CORE_SHAPE_SHAPELIBRARY_H

#include "core/detector/DetectorEvent.h"
#include "core/peak/Peak3D.h"
#include "core/shape/Profile1D.h"
#include "core/shape/Profile3D.h"

#include <array>

namespace nsx {

//! Parameters for building the shape library
struct ShapeLibParameters
{
    double detector_range_min = 1.5;     //! Minimum detector range (filter)
    double detector_range_max = 50.0;    //! Maximum detector range (filter)
    double peak_scale = 3.0;             //!
    double background_range_min = 3.0;   //! Start of background range in sigmas
    double background_range_max = 4.5;   //! End of background range in sigmas
    double strength_min = 1.0;           //! Minimum peak strength I/sigma (filter)
    bool kabsch_coords = true;           //! Are we using Kabsch or detector coordinates?
    int nbins_x = 20;                    //! Number of x histogram bins for peak
    int nbins_y = 20;                    //! Number of y histogram bins for peak
    int nbins_z = 20;                    //! Number of z histogram bins for peak
    double sigma_divergence = 0.33;      //! variance arising from beam divergence
    double sigma_mosaicity = 0.23;       //! variance arising from crystal mosaicity
};

//! Parameters for peak prediction
struct PredictionParameters
{
    double detector_range_min = 1.5;     //! Minimum detector range (filter)
    double detector_range_max = 50.0;    //! Maximum detector range (filter)
    double neighbour_max_radius = 100.0; //! Maximum radius for neighbouring peak search
    int min_n_neighbors = 400;           //! Minimum number of neighbours required for shape library
    double frame_range_max = 20.0;       //! Maximum angular separation of peaks in frames
};

class ShapeLibrary;

using sptrShapeLibrary = std::shared_ptr<ShapeLibrary>;

enum class PeakInterpolation { NoInterpolation = 0, InverseDistance = 1, Intensity = 2 };

struct FitData;

//! Helper function for predicting peaks
std::vector<Peak3D*> predictPeaks(
    ShapeLibrary* library, sptrDataSet data, UnitCell* unit_cell, double dmin, double dmax,
    double radius, double nframes, int min_neighbors, PeakInterpolation interpolation);

//! Store a library of peak shapes, to be used for peak prediction and integration.

//! The library stores a list of reference peaks. For each reference peak, the
//! library stores the covariance matrix of the intensity distribution, as well
//! as 3d- and 1d- integrated profiles. The covariance matrices are used to
//! roughly predict the shapes of weak peaks, and the integrated profiles are
//! used in the profile-fitting integration methods.

class ShapeLibrary {
 public:
    //! Construct an empty library.
    //! @param detector_coords if true, store profiles in detector coordinates;
    //! otherwise store in Kabsch coordinates
    ShapeLibrary();
    ShapeLibrary(bool detector_coords, double peakScale, double bkgBegin, double bkgEnd);

    //! Returns whether the library is stored in detector coords or Kabsch coords
    bool detectorCoords() const;

    //! Add a reference peak to the library
    bool addPeak(Peak3D* peak, Profile3D&& profile, Profile1D&& integrated_profile);

    //! Update the fitted covariances
    void updateFit(int num_iterations);

    //! Predict the (detector space) covariance of a given peak
    Eigen::Matrix3d predictCovariance(Peak3D* peak) const;

    //! Returns mean Pearson coefficient to measure quality of fit
    double meanPearson() const;

    //! Returns the average peak profile near the given detector event
    Profile3D meanProfile(const DetectorEvent& ev, double radius, double nframes) const;

    //! Returns the average peak profile near the given detector event
    std::vector<Intensity>
    meanProfile1D(const DetectorEvent& ev, double radius, double nframes) const;

    //! Returns the average peak covariance near the given detector event
    Eigen::Matrix3d meanCovariance(
        Peak3D* reference_peak, double radius, double nframes, size_t min_neighbors,
        PeakInterpolation interpolation) const;

    //! Find neighbors of a given peak
    std::vector<Peak3D*>
    findNeighbors(const DetectorEvent& ev, double radius, double nframes) const;

    //! Returns the peak scale used for the library
    double peakScale() const;

    //! Returns the background begin used for the library
    double bkgBegin() const;

    //! Returns the background end used for the library
    double bkgEnd() const;

    //! Returns the background end used for the library
    std::array<double, 6> choleskyD() const;

    //! Returns the background end used for the library
    std::array<double, 6> choleskyM() const;

    //! Returns the background end used for the library
    std::array<double, 6> choleskyS() const;

    //! Returns the background end used for the library
    std::map<Peak3D*, std::pair<Profile3D, Profile1D>> profiles() const;

 private:
    //! Predict the (detector space) covariance given the fit data
    Eigen::Matrix3d predictCovariance(const FitData& data) const;

    //! List of reference peak profiles
    std::map<Peak3D*, std::pair<Profile3D, Profile1D>> _profiles;

    //! Components of the Cholesky factor of beam divergence covariance matrix
    std::array<double, 6> _choleskyD;

    //! Components of the Cholesky factor of mosaicity covariance matrix
    std::array<double, 6> _choleskyM;

    //! Components of the Cholesky factor of shape covariance matrix
    std::array<double, 6> _choleskyS;

    //! Sets true if the profiles are stored in detector space coordinates or false
    //! for Kabsch coords
    bool _detectorCoords;

    //! The peak scale used by the library for integration
    double _peakScale;

    //! The background begin used by the library for integration
    double _bkgBegin;

    //! The background end used by the library for integration
    double _bkgEnd;
};

} // namespace nsx

#endif // CORE_SHAPE_SHAPELIBRARY_H
