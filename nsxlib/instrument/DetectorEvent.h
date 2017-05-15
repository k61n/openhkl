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

#ifndef NSXTOOL_DETECTOREVENT_H_
#define NSXTOOL_DETECTOREVENT_H_

#include <vector>
#include <Eigen/Core>

#include "DetectorState.h"

namespace nsx {
namespace Instrument {

// Forward declaration of detector class
class Detector;

class DetectorEvent {
public:
    // Constructor
    DetectorEvent(const Detector& detector, double x, double y, DetectorState values = {});
    //! Copy constructor
    DetectorEvent(const DetectorEvent& other);
    //! Move constructor
    DetectorEvent(DetectorEvent&& other);
    //! Assignment operator
    DetectorEvent& operator=(const DetectorEvent& other);
    //! Destructor
    ~DetectorEvent();

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

    /**
     *  @brief Get the transferred wavenumber for an event on a detector
     *  @param px horizontal position of the scattering event in pixels unit
     *  @param py vertical position of the scattering event in pixels units
     *  @param si incident wavenumber si=\f$ \frac{k_i}{2\pi} \f$
     *  @param from Optional scattering point position
     *  @return Transferred wavenumber s=\f$ \frac{k_f-k_i}{2\pi} \f$
     */
     Eigen::Vector3d getQ(double wave,const Eigen::Vector3d& from = Eigen::Vector3d::Zero()) const;

     /**
      *  @brief Get the scattering angles for an event on the detector
      *  @param px horizontal position of the scattering event in pixels unit
      *  @param py vertical position of the scattering event in pixels units
      *  @param gamma reference to angle in the yx-plane (gamma=0 along y)
      *  @param nu reference to elevation angle
      */
     void getGammaNu(double& gamma, double& nu,const Eigen::Vector3d& from=Eigen::Vector3d::Zero()) const;

     /**
      *  @brief Get the position of the scattering event.
      *  @param px horizontal position of the scattering event in pixels unit
      *  @param py vertical position of the scattering event in pixels units
      *  @return spatial position of this event
      */
     Eigen::Vector3d getPixelPosition() const;

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
} // end namespace nsx
#endif /* NSXTOOL_DETECTOREVENT_H_ */
