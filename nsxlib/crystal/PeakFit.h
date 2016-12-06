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

#ifndef NSXTOOL_PEAKFIT_H_
#define NSXTOOL_PEAKFIT_H_

#include "UnitCell.h"
#include "IShape.h"
#include "Peak3D.h"
#include <Eigen/Dense>

namespace SX
{

namespace Utils {
class IMinimizer;
}

namespace Crystal
{

class PeakFit
{
public:
    PeakFit(sptrPeak3D peak);
    ~PeakFit();

    int residuals(const Eigen::VectorXd& params, Eigen::VectorXd& res) const;
    int numParams() const;
    int numValues() const;

    Eigen::MatrixXd peakData(int frame) const;
    Eigen::MatrixXd predict(int frame) const;
    Eigen::MatrixXd predict(const Eigen::VectorXd& params, int frame) const;
    Eigen::MatrixXd chi2(int frame) const;
    Eigen::MatrixXd relDifference(int frame) const;

    double maxIntensity() const;

    Eigen::VectorXd defaultParams() const;

    bool fit(SX::Utils::IMinimizer& minimizer);

private:
    sptrPeak3D _peak;
    std::vector<Eigen::ArrayXXd> _peakData;
    std::vector<Eigen::ArrayXXd> _maskData;
    int _frameBegin, _frameEnd, _frames;
    int _rowMin, _rowMax;
    int _colMin, _colMax;
    Eigen::VectorXd _params;
};

} // namespace Crystal

} // namespace SX

#endif /* NSXTOOL_PEAKFIT_H_ */
