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

#include "../crystal/Peak3D.h"
#include "../data/DataSet.h"
#include "../instrument/Diffractometer.h"
#include "../instrument/Source.h"
#include "../instrument/InstrumentState.h"
#include "Detector.h"
#include "DetectorEvent.h"
#include "Gonio.h"

namespace nsx {


DetectorEvent::DetectorEvent(sptrDataSet data, double px, double py, double frame):
    _data(data), _px(px), _py(py), _frame(frame)
{
}

DetectorEvent::DetectorEvent(sptrPeak3D peak):
    _data(peak->data()), _px(), _py(), _frame()
{
    auto c = peak->getShape().center();
    _px = c[0];
    _py = c[1];
    _frame = c[2];
}

ReciprocalVector DetectorEvent::Kf() const
{
    auto source = _data->getDiffractometer()->getSource();
    double wavelength = source->getSelectedMonochromator().getWavelength();
    auto state = _data->getInterpolatedState(_frame);
    Eigen::Vector3d kf = static_cast<const Eigen::Vector3d&>(getPixelPosition()) - state.sample.getPosition();
    kf.normalize();
    kf /= wavelength;
    return ReciprocalVector(kf);
}

DirectVector DetectorEvent::getPixelPosition() const
{
    auto detector = _data->getDiffractometer()->getDetector();
    Eigen::Vector3d v = detector->getPos(_px, _py);
    auto gonio = detector->getGonio();

    // No gonio and no values set
    if (gonio == nullptr) {
        return DirectVector(v);
    }

    InstrumentState state = _data->getInterpolatedState(_frame);
    gonio->transformInPlace(v, state.detector.getValues());
    return DirectVector(v);
}

void DetectorEvent::getGammaNu(double& gamma, double& nu) const
{
    auto kf = static_cast<const Eigen::RowVector3d&>(Kf());
    gamma = std::atan2(kf[0], kf[1]);
    nu = std::asin(kf[2] / kf.norm());
}

Eigen::Vector3d DetectorEvent::coordinates() const
{
    return {_px, _py, _frame};
}

double DetectorEvent::getLorentzFactor() const
{
    double gamma,nu;
    getGammaNu(gamma, nu);
    double lorentz = 1.0/(sin(std::fabs(gamma))*cos(nu));
    return lorentz;
}

double DetectorEvent::get2Theta(const ReciprocalVector& si) const
{
    auto kf = static_cast<const Eigen::RowVector3d&>(Kf());
    auto ki = static_cast<const Eigen::RowVector3d&>(si);
    
    double proj = kf.dot(ki);
    return acos(proj/kf.norm()/ki.norm());
}


} // end namespace nsx
