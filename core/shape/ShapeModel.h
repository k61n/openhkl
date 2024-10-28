//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/shape/ShapeModel.h
//! @brief     Defines classes PeakInterpolation, ShapeModel
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_SHAPE_SHAPECOLLECTION_H
#define OHKL_CORE_SHAPE_SHAPECOLLECTION_H

#include "core/data/DataTypes.h"
#include "core/integration/IIntegrator.h"
#include "core/shape/Profile.h"
#include "core/shape/Profile1D.h"
#include "core/shape/Profile3D.h"

#include <array>
#include <set>

namespace ohkl {

class DetectorEvent;
class Peak3D;
class PeakCollection;
enum class Level;

/*! \addtogroup python_api
 *  @{*/

//! Interpolation type for assigning a shape to a predicted peaks
enum class PeakInterpolation { NoInterpolation = 0, InverseDistance = 1, Intensity = 2 };

//! Parameters for building the shape collection
struct ShapeModelParameters : public IntegrationParameters {
    double d_min = 1.5; //!< Minimum d (filter)
    double d_max = 50.0; //!< Maximum d (filter)
    double strength_min = 1.0; //!< Minimum peak strength I/sigma (filter)
    double strength_max = 1.0e7; //!< Maximum peak strength I/sigma (filter)
    bool kabsch_coords = true; //!< Are we using Kabsch or detector coordinates?
    int nbins_x = 20; //!< Number of x histogram bins for peak
    int nbins_y = 20; //!< Number of y histogram bins for peak
    int nbins_z = 10; //!< Number of z histogram bins for peak
    int n_subdiv = 1; //!< Number subdivisions along each axis per pixel
    int min_n_neighbors = 10; //!< Minimum number of neighbours required for shape collection
    double sigma_m = 0.1; //!< Variance due to crystal mosaicity
    double sigma_d = 0.1; //!< Variance due to beam divergence
    PeakInterpolation interpolation = PeakInterpolation::NoInterpolation;
    double neighbour_range_pixels = 100.0; //! Search radius for neighbouring profiles (pixels)
    double neighbour_range_frames = 20.0; //! Search radius for neighbouring profiles (frames)

    void log(const Level& level) const;
};

class ShapeModel;

using sptrShapeModel = std::shared_ptr<ShapeModel>;

struct FitData;

/*! \brief Store a collection of peak shapes,  for peak prediction and integration.
 *
 * The collection stores a list of reference peaks. For each reference peak, the
 * collection stores the covariance matrix of the intensity distribution, as well
 * as 3d- and 1d- integrated profiles. The covariance matrices are used to
 * roughly predict the shapes of weak peaks, and the integrated profiles are
 * used in the profile-fitting integration methods.
 */

class ShapeModel {
 public:
    //! Construct an empty collection.
    //! @param detector_coords if true, store profiles in detector coordinates;
    //! otherwise store in Kabsch coordinates
    ShapeModel(const sptrDataSet data);
    ShapeModel(const std::string& name, const sptrDataSet data);
    ShapeModel(const ShapeModelParameters& params, const sptrDataSet data);

    //! Get the integer id
    unsigned int id() const { return _id; };
    //! Set the integer id
    void setId(unsigned int id);

    //! Set the name
    void setName(const std::string& name) { _name = name; };
    //! Get the name
    std::string name() { return _name; };
    //! Get the associated DataSet
    sptrDataSet data() const { return _data; };

    //! Add a reference peak to the collection
    bool addPeak(Peak3D* peak, Profile3D&& profile, Profile1D&& integrated_profile);

    //! Update the fitted covariances
    void updateFit(int num_iterations = 1000);

    //! Set the shape collection parameters
    void setParameters(const ShapeModelParameters& params);

    //! Set shapes of a predicted peak collection
    void setPredictedShapes(PeakCollection* peaks, bool thread_parallel = true);

    //! Predict the (detector space) covariance of a given peak
    Eigen::Matrix3d predictCovariance(Peak3D* peak) const;

    //! Returns mean Pearson coefficient to measure quality of fit
    double meanPearson() const;

    //! Returns the average or nearest peak profile near the given detector event
    Profile* meanProfile(const DetectorEvent& ev) const;

    //! Returns the average or nearest peak profile near the given detector event
    Profile* meanProfile1D(const DetectorEvent& ev) const;

    //! Returns the average or nearest peak covariance near the given detector event
    Eigen::Matrix3d meanCovariance(Peak3D* reference_peak) const;

    //! Find neighbors of a given peak, or the nearest peak if there are none within the cutoff
    std::vector<Peak3D*> findNeighbors(const DetectorEvent& ev) const;

    //! Returns the background end used for the collection
    std::array<double, 6> choleskyD() const;

    //! Returns the background end used for the collection
    std::array<double, 6> choleskyM() const;

    //! Returns the background end used for the collection
    std::array<double, 6> choleskyS() const;

    //! Returns the background end used for the collection
    std::map<Peak3D*, Profile> profiles() const;

    //! Return number of peaks in collection
    int numberOfPeaks() const { return _profiles.size(); };

    //! Shape collection parameters
    ShapeModelParameters* parameters();

    //! Whether the collection uses Kabsch (f) or detector(t) coordinates
    bool detectorCoords() const;

    //! Set the progress handler
    void setHandler(sptrProgressHandler handler);

 private:
    //! integer id
    unsigned int _id;
    //! name
    std::string _name;
    //! Predict the (detector space) covariance given the fit data
    Eigen::Matrix3d predictCovariance(const FitData&) const;

    //! List of reference peak profiles
    std::map<Peak3D*, Profile> _profiles;

    //! Associated DataSet
    sptrDataSet _data;

    //! Components of the Cholesky factor of beam divergence covariance matrix
    std::array<double, 6> _choleskyD;

    //! Components of the Cholesky factor of mosaicity covariance matrix
    std::array<double, 6> _choleskyM;

    //! Components of the Cholesky factor of shape covariance matrix
    std::array<double, 6> _choleskyS;

    //! Shape model parameters
    ShapeModelParameters _params;

    //! Progress handler
    sptrProgressHandler _handler;
};

/*! @}*/
} // namespace ohkl

#endif // OHKL_CORE_SHAPE_SHAPECOLLECTION_H
