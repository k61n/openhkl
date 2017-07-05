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

#ifndef NSXLIB_PEAKFIT_H
#define NSXLIB_PEAKFIT_H

#include <Eigen/Dense>

#include "../crystal/Peak3D.h"
#include "../crystal/UnitCell.h"

namespace nsx {

class Minimizer;

class PeakFit {
public:
    PeakFit(sptrPeak3D peak);
    ~PeakFit();

    int residuals(const Eigen::VectorXd& params, Eigen::VectorXd& res) const;
    int numParams() const;
    int numValues() const;

    Eigen::ArrayXXd peakData(double frame) const;
    Eigen::ArrayXXd predict(double frame) const;
    Eigen::ArrayXXd background(const Eigen::VectorXd& params, double frame) const;
    Eigen::ArrayXXd predict(const Eigen::VectorXd& params, double frame) const;
    Eigen::ArrayXXd mask(double frame) const;
    Eigen::ArrayXXd chi2(std::size_t frame) const;
    Eigen::ArrayXXd relDifference(std::size_t frame) const;

    double maxIntensity() const;

    Eigen::VectorXd defaultParams() const;

    bool fit(Minimizer& minimizer);

    int frameBegin() const;
    int frameEnd() const;

    int integerFrame(double f) const;

private:
    sptrPeak3D _peak;
    std::vector<Eigen::ArrayXXd> _peakData;
    std::vector<Eigen::ArrayXXd> _maskData;
    std::size_t _frameBegin, _frameEnd, _frames;
    std::size_t _rowMin, _rowMax;
    std::size_t _colMin, _colMax;
    Eigen::VectorXd _params;
};

} // end namespace nsx

#endif // NSXLIB_PEAKFIT_H
