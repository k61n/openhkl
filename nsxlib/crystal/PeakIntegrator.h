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

#include <Eigen/Dense>

#include "Blob3D.h"
#include "GeometryTypes.h"
#include "IntegrationRegion.h"
#include "Intensity.h"
#include "Maybe.h"

namespace nsx {

class DataSet;

//! \class PeakIntegrator
//! \breif This is a helper class to handle per-frame integration of a peak.
class PeakIntegrator {
public:

    PeakIntegrator() = default;

    //! Create a PeakIntegrator instance from a given IntegrationRegion and DataSet.
    PeakIntegrator(const IntegrationRegion& region, const DataSet& data);
    ~PeakIntegrator() {}

    void step(const Eigen::MatrixXi& frame, size_t idx, const Eigen::MatrixXi& mask);
    void end();

    const Eigen::ArrayXd& getProjectionPeak() const;
    const Eigen::ArrayXd& getProjectionBackground() const;
    const Eigen::ArrayXd& getProjection() const;
    const Eigen::ArrayXd getPeakError() const;

    Intensity getMeanBackground() const;

    const IntegrationRegion& getRegion() const;

    Intensity getPeakIntensity() const;

    double pValue() const;

    Eigen::ArrayXd countsPeak() const;
    Eigen::ArrayXd pointsPeak() const;
    Eigen::ArrayXd pointsBkg() const;

    Eigen::ArrayXd shellIntensity() const;
    Eigen::ArrayXd shellPoints() const;

    #ifndef SWIG
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    #endif

private:
    IntegrationRegion _region;
    Eigen::Vector3d _lower;
    Eigen::Vector3d _upper;

    size_t _data_start;
    size_t _data_end;

    size_t _start_x;
    size_t _end_x;

    size_t _start_y;
    size_t _end_y;

    Eigen::Vector3d _point1;

    int _dx;
    int _dy;

    Eigen::ArrayXd _projection;
    Eigen::ArrayXd _projectionPeak;
    Eigen::ArrayXd _projectionBkg;

    Eigen::ArrayXXd _shellIntensity;
    Eigen::ArrayXXd _shellIntensitySquared;
    Eigen::ArrayXXd _shellPoints;
};

} // end namespace nsx
