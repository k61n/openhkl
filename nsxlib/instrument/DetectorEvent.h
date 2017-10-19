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

#ifndef NSXLIB_DETECTOREVENT_H
#define NSXLIB_DETECTOREVENT_H

#include <vector>

#include <Eigen/Core>

#include "../data/DataTypes.h"
#include "../geometry/DirectVector.h"
#include "../geometry/ReciprocalVector.h"
#include "../instrument/InstrumentTypes.h"

namespace nsx {

//! Class to register an event on a detector, recording the instrument state
class DetectorEvent {
public:
    //! Construct a detector event
    DetectorEvent(sptrDataSet data, double px, double py, double frame);

    /**
     *  @brief Get 2\f$ \theta \f$
     *  @param px horizontal position of the scattering event in pixels unit
     *  @param py vertical position of the scattering event in pixels units
     *  @param si Incident wavenumber
     */
    double get2Theta(const ReciprocalVector& si) const;

    //! Return outgoing momentum (in lab coordinates) of the outgoing neutron.
    ReciprocalVector Kf() const;

     /**
      *  @brief Get the scattering angles for an event on the detector
      *  @param px horizontal position of the scattering event in pixels unit
      *  @param py vertical position of the scattering event in pixels units
      *  @param gamma reference to angle in the yx-plane (gamma=0 along y)
      *  @param nu reference to elevation angle
      */
    void getGammaNu(double& gamma, double& nu) const;

    //! Return real (lab) space position of the detector event p = (x, y, frame)
    DirectVector getPixelPosition() const;

    Eigen::Vector3d coordinates() const;

private:
    sptrDataSet _data;
    double _px, _py, _frame;
};

} // end namespace nsx

#endif // NSXLIB_DETECTOREVENT_H
