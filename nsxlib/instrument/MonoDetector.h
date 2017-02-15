/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2012- Laurent C. Chapon Institut Laue-Langevin
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr

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

#ifndef NSXTOOL_MONODETECTOR_H_
#define NSXTOOL_MONODETECTOR_H_

#include <string>

#include "Detector.h"
#include "../utils/Types.h"

namespace SX {
namespace Instrument {

/** @brief Base class for Mono Detectors.
 *
 *
 */
class MonoDetector : public Detector
{
public:

    //! Static constructor of a MonoDetector from a property tree node
    static Detector* create(const proptree::ptree& node);

    //! Construct a default MonoDetector
    MonoDetector();
    //! Construct a MonoDetector from another one
    MonoDetector(const MonoDetector& other);
    //! Construct a MonoDetector with a given name
    MonoDetector(const std::string& name);
    //! Constructs a MonoDetector from a property tree node
    MonoDetector(const proptree::ptree& node);
    // Destructor
    virtual ~MonoDetector()=0;

    //! Return a pointer to a copy of the MonoDetector (not implemented)
    virtual Detector* clone() const=0;

    //! Assignment operator
    virtual MonoDetector& operator=(const MonoDetector& other);

    //! Return the number of pixels of the detector
    unsigned int getNPixels() const;
    //! Return the number of columns of the detector
    unsigned int getNCols() const;
    //! Set the number of columns of the detector
    void setNCols(unsigned int cols);
    //! Return the number of rows of the detector
    unsigned int getNRows() const;
    //! Set the number of rows
    void setNRows(unsigned int rows);
    //! Set the pixel origin of the detector
    void setOrigin(double px, double py);
    //!  Get the minimum row index
    int getMinRow() const;
    //!  Get the maximum row index
    int getMaxRow() const;
    //!  Get the minimum col index
    int getMinCol() const;
    //!  Get the maximum col index
    int getMaxCol() const;
    //! Set the number of pixels of the detector
    void setNPixels(unsigned int cols,unsigned int rows);
    //! Return true whether a given pixel falls inside the detector
    bool hasPixel(double px, double py) const;

    //! Return the height of a detector pixel
    double getPixelHeigth() const;
    //! Return the width of a detector pixel
    double getPixelWidth() const;

    //! Return the width of the detector (meters)
    double getHeight() const;
    //! Set the height of the detector (meters)
    virtual void setHeight(double height)=0;
    //! Return the width of the detector (meters)
    double getWidth() const;
    //! Set the width of the detector (meters)
    virtual void setWidth(double width)=0;
    //! Set the dimensions of the detector (meters).
    void setDimensions(double width, double height);

    //! Return the height in angular units of the detector (radians)
    double getAngularHeight() const;
    //! Set the height in angular units of the detector (radians)
    virtual void setAngularHeight(double angle)=0;
    //! Return the width in angular units of the detector (radians)
    double getAngularWidth() const;
    //! Set the height in angular units of the detector (radians)
    virtual void setAngularWidth(double angle)=0;
    //! Set the angular dimensions of the detector (radians)
    void setAngularDimensions(double wAngle, double hAngle);

    //! Return the sample to detector distance (meters)
    double getDistance() const;
    //! Set sample to detector distance
    void setDistance(double d);
    //! Set the rest position of the detector (along y in Busing Levy convention)
    void setRestPosition(const Eigen::Vector3d& p);

    //! Returns the number of detector
    unsigned int getNDetectors() const;

protected:
    SX::Types::uint _nRows, _nCols;
    double _minRow, _minCol;
    double _width, _height;
    double _angularWidth, _angularHeight;
    double _distance;
};

} // end namespace Instrument
} // end namespace SX

#endif /* NSXTOOL_MONODETECTOR_H_ */
