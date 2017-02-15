/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2012- Laurent C. Chapon Eric Pellegrini
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
#ifndef NSXTOOL_DETECTOREVENT_H_
#define NSXTOOL_DETECTOREVENT_H_

#include <vector>
#include <Eigen/Core>

namespace SX {
namespace Instrument {

// Forward declaration of detector class
class Detector;

class DetectorEvent {
public:
    // Constructor
    DetectorEvent(const Detector* detector, double x, double y, std::vector<double> values = {});
    //! Copy constructor
    DetectorEvent(const DetectorEvent& other);
    //! Move constructor
    DetectorEvent(DetectorEvent&& other);
    //! Assignment operator
    DetectorEvent& operator=(const DetectorEvent& other);
    //! Destructor
    ~DetectorEvent();
    //! return a pointer to the detector related to this detector event
    const Detector* getParent() const;

    const std::vector<double>& getValues() const;
    double getX() const;
    double getY() const;

    /**
     *  @brief Get 2\f$ \theta \f$
     *  @param px horizontal position of the scattering event in pixels unit
     *  @param py vertical position of the scattering event in pixels units
     *  @param si Incident wavenumber
     */
    double get2Theta(const Eigen::Vector3d& si = Eigen::Vector3d(0,1,0)) const;

    /**
     * Get the scattered wavenumber for an event on this detector
     */
    Eigen::Vector3d getKf(double wave,const Eigen::Vector3d& from = Eigen::Vector3d::Zero()) const;
private:
    //! Default constructor
    // DetectorEvent();
    // friend class Detector;
    const Detector* _detector;
    //! Position of the event on the detector
    double _x, _y;
    //! Setup of the detector Gonio
    std::vector<double> _values;

};

} // Namespace Instrument
}  // Namespace SX
#endif /* NSXTOOL_DETECTOREVENT_H_ */
