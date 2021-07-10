//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/shape/ShapeCollection.h
//! @brief     Defines classes PeakInterpolation, ShapeCollection
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_SHAPE_SHAPECOLLECTION_H
#define NSX_CORE_SHAPE_SHAPECOLLECTION_H

#include "base/utils/ProgressHandler.h"
#include "core/shape/IPeakIntegrator.h"
#include "core/shape/Profile1D.h"
#include "core/shape/Profile3D.h"

#include <array>

namespace nsx {

class DetectorEvent;
class Peak3D;
class PeakCollection;
enum class Level;

//! Parameters for building the shape collection
struct ShapeCollectionParameters : public IntegrationParameters {
    double d_min = 1.5; //!< Minimum d (filter)
    double d_max = 50.0; //!< Maximum d (filter)
    double strength_min = 1.0; //!< Minimum peak strength I/sigma (filter)
    double strength_max = 1.0e7; //!< Maximum peak strength I/sigma (filter)
    bool kabsch_coords = true; //!< Are we using Kabsch or detector coordinates?
    int nbins_x = 20; //!< Number of x histogram bins for peak
    int nbins_y = 20; //!< Number of y histogram bins for peak
    int nbins_z = 10; //!< Number of z histogram bins for peak
    int min_n_neighbors = 10; //!< Minimum number of neighbours required for shape collection

    void log(const Level& level) const;
};

//! Parameters for peak prediction
struct PredictionParameters : public IntegrationParameters {
    double d_min = 1.5; //!< Minimum detector range (filter)
    double d_max = 50.0; //!< Maximum detector range (filter)

    void log(const Level& level) const;
};

class ShapeCollection;

using sptrShapeCollection = std::shared_ptr<ShapeCollection>;

enum class PeakInterpolation { NoInterpolation = 0, InverseDistance = 1, Intensity = 2 };

struct FitData;

//! Helper function for predicting peaks
std::vector<Peak3D*> predictPeaks(
    const sptrDataSet data, const UnitCell* unit_cell, const PredictionParameters* params,
    sptrProgressHandler handler = nullptr);

//! Store a collection of peak shapes, to be used for peak prediction and integration.

//! The collection stores a list of reference peaks. For each reference peak, the
//! collection stores the covariance matrix of the intensity distribution, as well
//! as 3d- and 1d- integrated profiles. The covariance matrices are used to
//! roughly predict the shapes of weak peaks, and the integrated profiles are
//! used in the profile-fitting integration methods.

class ShapeCollection {
 public:
    //! Construct an empty collection.
    //! @param detector_coords if true, store profiles in detector coordinates;
    //! otherwise store in Kabsch coordinates
    ShapeCollection();
    ShapeCollection(bool detector_coords, double peakEnd, double bkgBegin, double bkgEnd);

    //! Returns whether the collection is stored in detector coords or Kabsch coords
    bool detectorCoords() const;

    //! Add a reference peak to the collection
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
    std::vector<Intensity> meanProfile1D(
        const DetectorEvent& ev, double radius, double nframes) const;

    //! Returns the average peak covariance near the given detector event
    Eigen::Matrix3d meanCovariance(
        Peak3D* reference_peak, double radius, double nframes, size_t min_neighbors,
        PeakInterpolation interpolation) const;

    //! Find neighbors of a given peak
    std::vector<Peak3D*> findNeighbors(
        const DetectorEvent& ev, double radius, double nframes) const;

    //! Set the peak shapes for a peak collection
    void setPredictedShapes(
        PeakCollection* peaks, PeakInterpolation interpolation,
        sptrProgressHandler handler = nullptr);

    //! Returns the peak scale used for the collection
    double peakEnd() const;

    //! Returns the background begin used for the collection
    double bkgBegin() const;

    //! Returns the background end used for the collection
    double bkgEnd() const;

    //! Returns the background end used for the collection
    std::array<double, 6> choleskyD() const;

    //! Returns the background end used for the collection
    std::array<double, 6> choleskyM() const;

    //! Returns the background end used for the collection
    std::array<double, 6> choleskyS() const;

    //! Returns the background end used for the collection
    std::map<Peak3D*, std::pair<Profile3D, Profile1D>> profiles() const;

    //! Return number of peaks in collection
    int numberOfPeaks() const { return _profiles.size(); };

    //! Return number of peaks with no neighbours
    int nLonelyPeaks() const { return _n_lonely_peaks; };

    //! Return number of peaks with too few neighbours
    int nUnfriendlyPeaks() const { return _n_unfriendly_peaks; };

    //! Return number of failed interpolations
    int nFailedInterp() const { return _n_failed_interp; };

    //! Return number of cases of no neighbouring profiles
    int nNoProfile() const { return _n_no_profile; };

    //! Shape collection parameters
    ShapeCollectionParameters* parameters();

 private:
    //! Predict the (detector space) covariance given the fit data
    Eigen::Matrix3d predictCovariance(const FitData&) const;

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

    //! The peak scale used by the collection for integration
    double _peakEnd;

    //! The background begin used by the collection for integration
    double _bkgBegin;

    //! The background end used by the collection for integration
    double _bkgEnd;

    //! Number of failed interpolations
    mutable int _n_failed_interp = 0;

    //! Number of peaks with no neighbours
    mutable int _n_lonely_peaks = 0;

    //! Number of peaks with too few neighbours
    mutable int _n_unfriendly_peaks = 0;

    //! Number of peaks with no neighbouring profiles
    mutable int _n_no_profile = 0;

    //! Shape collection parameters
    std::shared_ptr<ShapeCollectionParameters> _params;
};

} // namespace nsx

#endif // NSX_CORE_SHAPE_SHAPECOLLECTION_H
