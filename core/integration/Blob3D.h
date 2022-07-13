//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/integration/Blob3D.h
//! @brief     Defines class Blob3D
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef NSX_CORE_INTEGRATION_BLOB3D_H
#define NSX_CORE_INTEGRATION_BLOB3D_H

#include <Eigen/Dense>
#include <iostream> // needed in no-swig case (?)

namespace ohkl {

/*! \addtogroup python_api
 *  @{*/

/*! \brief A region of interest in a 3D image.
 *
 *  A Blob is constructed by adding points in the image with coordinates x,y,z
 *  and an associated mass that represents any scalar field such as intensity.
 *  Blob objects records the total mass, the mass-weighted first and second
 *  moments as new points are added to the blob. Knowledge about individual
 *  points is lost, i.e Blob can only increase in size.Blobs can be merged and
 *  maintain zero, first and second momentum. Blob3D can be transformed into an
 *  Ellipsoid, by diagonalizing the variance tensor.
 */

class Blob3D {
 public:
    //! Initialize an empty blob
    Blob3D();
    //! Initialize a blob with a point of mass m at x,y,z
    Blob3D(double x, double y, double z, double m);
    //! Add point to the Blob
    void addPoint(double x, double y, double z, double m);
    //! Merge a second blob
    void merge(const Blob3D&);
    //! Returns the total mass
    double getMass() const;
    //! Returns the number of points
    int getComponents() const;
    //! Returns the minimumMass
    double getMinimumMass() const;
    //! Returns the minimumMass
    double getMaximumMass() const;
    //! Returns the center of Mass
    Eigen::Vector3d center() const;
    //! Gets the ellipsoid parameters
    bool toEllipsoid(
        double scale, Eigen::Vector3d& center, Eigen::Vector3d& eigenvalues,
        Eigen::Matrix3d& eigenvectors) const;
    //! Print to ostream
    void printSelf(std::ostream& os) const;
    //! Gets covariance matrix of the blob
    Eigen::Matrix3d covariance() const;
    //! Whether the blob is valid
    bool isValid() const;

 private:
    //! Zeroth moment (total mass)
    double _m0;
    //! First moments
    Eigen::Vector3d _m1;
    //! Second moments
    Eigen::Matrix3d _m2;
    //! Number of points contributing
    unsigned int _npoints;
    //! Minimum mass value
    double _minValue;
    //! Maximum mass value
    double _maxValue;
    //! Validity of blob
    mutable bool _valid;
};

#ifndef SWIG
std::ostream& operator<<(std::ostream& os, const Blob3D& b);
#endif

/*! @}*/
} // namespace ohkl

#endif // NSX_CORE_INTEGRATION_BLOB3D_H
