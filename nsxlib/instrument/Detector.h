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
#include "DetectorEvent.h"
#include "DirectVector.h"
#include "Enums.h"
#include "GeometryTypes.h"

namespace nsx {


//! \brief Base class for Detectors.
class Detector: public Component {
public:
    //! Static constructor of a Detector from a property tree node
    static Detector* create(const YAML::Node& node);

    //! Construct a Detector
    Detector();
    //! Construct a Detector from another Detector
    Detector(const Detector& other)=default;
    //! Construct a Detector with a given name
    Detector(const std::string& name);
    //! Constructs a sample from a property tree node
    Detector(const YAML::Node& node);
    //! Return a pointer to a copy of the Detector
    virtual Detector* clone() const=0;
    // Destructor
    virtual ~Detector()=0;

    //! Assignment operator
    Detector& operator=(const Detector& other)=default;

    //! Return the sample to detector distance (meters)
    double distance() const;
    //! Set sample to detector distance
    void setDistance(double d);

    //! Return the number of columns of the detector
    unsigned int nCols() const;
    //! Set the number of columns of the detector
    void setNCols(unsigned int cols);
    //! Return the number of rows of the detector
    unsigned int nRows() const;
    //! Set the number of rows
    void setNRows(unsigned int rows);

    //!  Get the minimum row index
    int minRow() const;
    //!  Get the maximum row index
    int maxRow() const;
    //!  Get the minimum col index
    int minCol() const;
    //!  Get the maximum col index
    int maxCol() const;

    //! Return true whether a given pixel is inside the detector
    bool hasPixel(double px, double py) const;

    //! Return the height of the detector (meters)
    double height() const;
    //! Set the height of the detector (meters)
    virtual void setHeight(double height)=0;

    //! Return the width of the detector (meters)
    double width() const;
    //! Set the width of the detector (meters)
    virtual void setWidth(double width)=0;

    //! Return the height in angular units of the detector (radians)
    double angularHeight() const;
    //! Set the height in angular units of the detector (radians)
    virtual void setAngularHeight(double angle)=0;

    //! Return the width in angular units of the detector (radians)
    double angularWidth() const;
    //! Set the height in angular units of the detector (radians)
    virtual void setAngularWidth(double angle)=0;

    //! Return the detector event (pixel x, pixel y, time of flight) associated with a given kf. 
    //! Returns with _negative_ tof if no such event is possible.
    virtual DetectorEvent constructEvent(const DirectVector& from, const ReciprocalVector& kf) const = 0;

    //! Returns the position of a given pixel in detector space. This takes into account the detector motions in detector space.
    virtual DirectVector pixelPosition(double x, double y) const=0;
    //! Return the mean detector pixel height
    double pixelHeight() const;
    //! Return the mean detector pixel width
    double pixelWidth() const;

    //! Returns how data are mapped to detector
    DataOrder dataOrder() const;

    //! Return the Jacobian matrix of the transformation (px,py) -> (x,y,z) from pixel coordinates to lab coordinates.
    //! The first and second columns are the derivatives of (x,y,z) with respect to px and py.
    //! The third column is identically zero and is kept only for convenience.
    virtual Eigen::Matrix3d jacobian(double x, double y) const = 0;

    //! Return the detector baseline. Measured count = gain * (neutron count) + baseline
    double baseline() const;
    //! Return the detector gain. Measured count = gain * (neutron count) + baseline
    double gain() const;

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

    DataOrder _dataorder;
    //! Detector baseline. Default is 0.0
    double _baseline;
    //! Detector gain. Default is 1.0
    double _gain;


};

} // end namespace nsx
