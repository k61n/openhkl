/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2017- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
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

#include "DataSet.h"
#include "Ellipsoid.h"
#include "Intensity.h"
#include "Peak3D.h"
#include "PeakCoordinateSystem.h"
#include "ShapeLibrary.h"
#include "GaussianIntegrator.h"

namespace nsx {

GaussianIntegrator::GaussianIntegrator():
    StrongPeakIntegrator()
{

}

bool GaussianIntegrator::compute(sptrPeak3D peak, const IntegrationRegion& region)
{
    if (!peak) {
        return false;
    }

    // first get mean background
    StrongPeakIntegrator::compute(peak, region);
    
    Eigen::Vector3d c = peak->getShape().center();
    Eigen::Matrix3d A = peak->getShape().metric();

    // todo....

    double sigma = _integratedIntensity.sigma();

    if (std::isnan(sigma) || sigma <= 0.0) {
        return false;
    }

    // TODO: rocking curve!

    return true;
}

} // end namespace nsx
