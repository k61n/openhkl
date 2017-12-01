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

#pragma once

#include <Eigen/Core>

#include "Ellipsoid.h"
#include "GeometryTypes.h"

namespace nsx {

class IntegrationRegion {
public:
    IntegrationRegion(Ellipsoid shape = {}, double bkg_begin = 1.0, double bkg_end = 3.0, int nslices = 10);

    void updateMask(Eigen::MatrixXi& mask, double z) const;

    AABB aabb() const;

    //! Classify the given point. Positive indicates it is in one of the integration shells,
    //! zero indicates it is in background, and negative indicates it is neither integration nor background.
    int classifySlice(const Eigen::Vector3d p) const;

    //! Number of slices used in integration
    int nslices() const;

    //! Best integration slice
    int bestSlice() const;

    //! Set the best integration slice.
    void setBestSlice(int n);

private:

    Ellipsoid _shape;

    double _bkgBegin;

    double _bkgEnd;

    int _nslices;

    int _bestSlice;
};

} // end namespace nsx
