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
#include <optional>
#include <set>

namespace nsx {

class DetectorEvent;
class Peak3D;
class PeakCollection;
enum class Level;

enum class PeakInterpolation { NoInterpolation = 0, InverseDistance = 1, Intensity = 2 };

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
    double sigma_m = 0.1; //!< Variance due to crystal mosaicity
    double sigma_d = 0.1; //!< Variance due to beam divergence
    PeakInterpolation interpolation = PeakInterpolation::NoInterpolation;

    void log(const Level& level) const;
};

class ShapeCollection;

using sptrShapeCollection = std::shared_ptr<ShapeCollection>;

struct FitData;

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
    ShapeCollection(std::shared_ptr<ShapeCollectionParameters> params);

    //! Add a reference peak to the collection
    bool addPeak(Peak3D* peak, Profile3D&& profile, Profile1D&& integrated_profile);

    //! Update the fitted covariances
    void updateFit(int num_iterations);

    //! Set the shape collection parameters
    void setParameters(std::shared_ptr<ShapeCollectionParameters> params);

    //! Set shapes of a predicted peak collection
    void setPredictedShapes(PeakCollection* peaks, PeakInterpolation interpolation);

    //! Predict the (detector space) covariance of a given peak
    Eigen::Matrix3d predictCovariance(Peak3D* peak) const;

    //! Returns mean Pearson coefficient to measure quality of fit
    double meanPearson() const;

    //! Returns the average peak profile near the given detector event
    std::optional<Profile3D> meanProfile(
        const DetectorEvent& ev, double radius, double nframes) const;

    //! Returns the average peak profile near the given detector event
    std::optional<std::vector<Intensity>> meanProfile1D(
        const DetectorEvent& ev, double radius, double nframes) const;

    //! Returns the average peak covariance near the given detector event
    std::optional<Eigen::Matrix3d> meanCovariance(
        Peak3D* reference_peak, double radius, double nframes, size_t min_neighbors,
        PeakInterpolation interpolation) const;

    //! Find neighbors of a given peak
    std::optional<std::vector<Peak3D*>> findNeighbors(
        const DetectorEvent& ev, double radius, double nframes) const;

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

    //! Shape collection parameters
    ShapeCollectionParameters* parameters();

    //! Whether the collection uses Kabsch (f) or detector(t) coordinates
    bool detectorCoords() const;

    //! Set the bounding box depending on the coordinate type
    AABB getAABB();

    //! Integrate the shape collection
    void integrate(
        std::vector<Peak3D*> peaks, std::set<nsx::sptrDataSet> datalist,
        sptrProgressHandler handler = nullptr);

    //! Set the progress handler
    void setHandler(sptrProgressHandler handler);

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

    //! Shape collection parameters
    std::shared_ptr<ShapeCollectionParameters> _params;

    //! Progress handler
    sptrProgressHandler _handler;
};

} // namespace nsx

#endif // NSX_CORE_SHAPE_SHAPECOLLECTION_H
