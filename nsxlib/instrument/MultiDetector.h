/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon, Eric Pellegrini
 Institut Laue-Langevin
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

#ifndef NSXTOOL_MULTIDETECTOR_H_
#define NSXTOOL_MULTIDETECTOR_H_

#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>

#include "../kernel/Composite.h"
#include "Detector.h"

namespace SX
{

namespace Instrument
{

class MultiDetector : public SX::Kernel::Composite<Detector,const proptree::ptree&>
{
public:

    //! Static constructor of a MultiDetector from a property tree node
    static Detector* create(const proptree::ptree& node);

    //! Construct a MultiDetector
    MultiDetector();
    //! Construct a MultiDetector from another MultiDetector
    MultiDetector(const MultiDetector& other);
    //! Construct a MultiDetector with a given name
    MultiDetector(const std::string& name);
    //! Constructs a MultiDetector from a property tree node
    MultiDetector(const proptree::ptree& node);
    //! Return a pointer to a copy of a MultiDetector
    Detector* clone() const;
    //! Destructor
    ~MultiDetector();

    //! Return the number of pixels of the detector.
    unsigned int getNPixels() const;
    //! Return the number of columns of the detector.
    unsigned int getNCols() const;
    //! Return the number of rows of the detector.
    unsigned int getNRows() const;
    //!  Get the minimum row index
    int getMinRow() const;
    //!  Get the maximum row index
    int getMaxRow() const;
    //!  Get the minimum col index
    int getMinCol() const;
    //!  Get the maximum col index
    int getMaxCol() const;
    //! Return true whether a given pixel falls inside the detector
    bool hasPixel(double px, double py) const;

    //! Return the height of the detector (meters)
    double getHeight() const;
    //! Return the width of the detector (meters)
    double getWidth() const;

    //! Return the angular height of the detector (radians)
    double getAngularHeight() const;
    //! Return the angular width of the detector (radians)
    double getAngularWidth() const;

    //! Returns the number of detector
    unsigned int getNDetectors() const;

    //! Returns the position of a given pixel in detector space. This takes into account the detector motions in detector space.
    Eigen::Vector3d getPos(double px, double py) const;

    //! Determine whether detector at rest can receive a scattering event with direction given by Kf. px and py are detector position if true.
    virtual bool hasKf(const Eigen::Vector3d& kf,const Eigen::Vector3d& from, double& px, double& py, double& t) const;

};

} // Namespace Instrument

} // Namespace SX

#endif /* NSXTOOL_MULTIDETECTOR_H_ */
