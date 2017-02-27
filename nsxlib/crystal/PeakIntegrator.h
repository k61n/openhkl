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

#ifndef NSXTOOL_PEAKINTEGRATOR_H_
#define NSXTOOL_PEAKINTEGRATOR_H_

#include "../geometry/IntegrationRegion.h"
#include <Eigen/Core>

namespace SX {

namespace Data {
class IData;
}

namespace Crystal {

class PeakIntegrator {
public:
    PeakIntegrator() = delete;
    PeakIntegrator(const SX::Geometry::IntegrationRegion& region, const SX::Data::IData& data);

    void step(const Eigen::MatrixXi& frame, size_t idx, const Eigen::MatrixXi& mask);
    void end();

    const Eigen::VectorXd& getProjectionPeak() const;
    const Eigen::VectorXd& getProjectionBackground() const;
    const Eigen::VectorXd& getProjection() const;

    const SX::Geometry::IntegrationRegion& getRegion() const;
private:

    SX::Geometry::IntegrationRegion _region;
    Eigen::Vector3d _lower;
    Eigen::Vector3d _upper;

    long _data_start;
    long _data_end;

    long _start_x;
    long _end_x;

    long _start_y;
    long _end_y;

    Eigen::Vector4d _point1;

    int _dx;
    int _dy;

    Eigen::VectorXd _projection;
    Eigen::VectorXd _projectionPeak;
    Eigen::VectorXd _projectionBkg;
    Eigen::VectorXd _pointsPeak;
    Eigen::VectorXd _pointsBkg;
    Eigen::VectorXd _countsPeak;
    Eigen::VectorXd _countsBkg;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};

} // namespace Crystal
} // namespace SX

#endif // NSXTOOL_PEAKINTEGRATOR_H_
