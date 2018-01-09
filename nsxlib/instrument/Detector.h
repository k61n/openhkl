/*
 nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Institut Laue-Langevin
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher
 -----------------------------------------------------------------------------------------

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#pragma once

#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include <Eigen/Dense>

#include "Component.h"
#include "DirectVector.h"
#include "Enums.h"

namespace nsx {


/** @brief Base class for Detectors.
 *
 *
 */
class Detector: public Component {
public:
    //! Static constructor of a Detector from a property tree node
    static Detector* create(const YAML::Node& node);

    //! Construct a Detector
    Detector();
    //! Construct a Detector from another Detector
    Detector(const Detector& other);
    //! Construct a Detector with a given name
    Detector(const std::string& name);
    //! Constructs a sample from a property tree node
    Detector(const YAML::Node& node);
    //! Return a pointer to a copy of the Detector
    virtual Detector* clone() const=0;
    // Destructor
    virtual ~Detector()=0;

    //! Assignment operator
    virtual Detector& operator=(const Detector& other);

    //! Return the number of pixels of the detector.
    virtual unsigned int getNPixels() const=0;
    //! Return the number of columns of the detector.
    virtual unsigned int getNCols() const=0;
    //! Return the number of rows of the detector.
    virtual unsigned int getNRows() const=0;
    //!  Get the minimum row index
    virtual int getMinRow() const=0;
    //!  Get the maximum row index
    virtual int getMaxRow() const=0;
    //!  Get the minimum col index
    virtual int getMinCol() const=0;
    //!  Get the maximum col index
    virtual int getMaxCol() const=0;
    //! Return true whether a given pixel falls inside the detector
    virtual bool hasPixel(double px, double py) const=0;

    //! Return the height of the detector (meters)
    virtual double getHeight() const=0;
    //! Return the width of the detector (meters)
    virtual double getWidth() const=0;

    //! Return the angular height of the detector (radians)
    virtual double getAngularHeight() const=0;
    //! Return the angular width of the detector (radians)
    virtual double getAngularWidth() const=0;

    //! Reuturn whether the detector with goniometer values can collect scattering at Kf. If true, px and py would be the pixel coordinates of the event
    bool receiveKf(double& px, double& py,const Eigen::Vector3d& kf,const Eigen::Vector3d& from, double& t) const;

    //! Returns the number of detector
    virtual unsigned int getNDetectors() const=0;

    //! Returns the position of a given pixel in detector space. This takes into account the detector motions in detector space.
    virtual DirectVector pixelPosition(double x, double y) const=0;

    //! Determine whether detector at rest can receive a scattering event with direction given by Kf. px and py are detector position if true.
    virtual bool hasKf(const Eigen::Vector3d& kf, const Eigen::Vector3d& from, double& px, double& py, double& t) const =0;

    //!
    DataOrder getDataOrder() const {return _dataorder;}

private:
    DataOrder _dataorder;
};

} // end namespace nsx
