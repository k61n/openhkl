/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2016- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forshungszentrum Juelich GmbH
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

#include <cmath>
#include <stdexcept>

#include "PeakFit.h"
#include "IData.h"
#include "IMinimizer.h"

namespace SX
{
using namespace Utils;

namespace Crystal
{

PeakFit::PeakFit(sptrPeak3D peak): _peak(peak)
{
    auto lower = _peak->getBackground()->getLower();
    auto upper = _peak->getBackground()->getUpper();

    _frameBegin = std::ceil(lower(2));
    _frameEnd = std::ceil(upper(2));

    _frameBegin = std::max(_frameBegin, 0);
    _frameEnd = std::min(_frameEnd, (int)_peak->getData()->getNFrames());

    _rowMin = std::ceil(lower(1));
    _rowMax = std::ceil(upper(1));

    _colMin = std::ceil(lower(0));
    _colMax = std::ceil(upper(0));

    const int nrows = _rowMax-_rowMin;
    const int ncols = _colMax-_colMin;

    _frames = _frameEnd - _frameBegin;
    _peakData.resize(_frames);
    _maskData.resize(_frames);

    for (int i = _frameBegin; i < _frameEnd; ++i) {
        auto frameData = _peak->getData()->getFrame(i);
        _peakData[i-_frameBegin] = frameData.block(_rowMin, _colMin, nrows, ncols).cast<double>();
        _maskData[i-_frameBegin].resize(nrows, ncols);

        for (unsigned int r = 0; r < nrows; ++r) {
            for (unsigned int c = 0; c < ncols; ++c) {
                Eigen::Vector4d p(c+_colMin, r+_rowMin, i, 1);
                _maskData[i-_frameBegin](r,c) = _peak->getBackground()->isInside(p) ? 1.0 : 0.0;
            }
        }

        _peakData[i-_frameBegin] *= _maskData[i-_frameBegin];
    }

    _params = defaultParams();
}

PeakFit::~PeakFit()
{

}

int PeakFit::residuals(const Eigen::VectorXd &params, Eigen::VectorXd &res) const
{
    assert(params.size() == numParams());

    res.resize(numValues());

    const double a = params(0);
    const Eigen::Vector3d b(params(1), params(2), params(3));
    const double c = params(4);

    int i = 0;

    for (int frame = _frameBegin; frame < _frameEnd; ++frame) {
        auto&& pred = predict(params, frame);

        for (int r = 0; r < _rowMax-_rowMin; ++r) {
            for (int c = 0; c < _colMax-_colMin; ++c) {
                res(i++) = pred(r, c);
            }
        }
    }

    return 0;
}

int PeakFit::numParams() const
{
    return _params.size();
}

int PeakFit::numValues() const
{
    return (_rowMax-_rowMin)*(_colMax-_colMin)*(_frameEnd-_frameBegin);
}

Eigen::MatrixXd PeakFit::peakData(int frame) const
{
    if (frame < _frameBegin)
        frame = _frameBegin;

    if (frame >= _frameEnd)
        frame = _frameEnd-1;

    return _peakData[frame-_frameBegin];
}

Eigen::MatrixXd PeakFit::predict(int frame) const
{
    return predict(_params, frame);
}

Eigen::MatrixXd PeakFit::predict(const Eigen::VectorXd &params, int frame) const
{
    const Eigen::VectorXd& p = params;

    int i = 0;

    double bkg = p(i++);
    double b0 = p(i++);
    double b1 = p(i++);
    double c = p(i++);
    double d00 = p(i++);
    double d01 = p(i++);
    double d02 = p(i++);
    double d11 = p(i++);
    double d12 = p(i++);
    double d22 = p(i++);
    double x0x = p(i++);
    double x0y = p(i++);
    double x0z = p(i++);

    Eigen::Vector3d v0(x0x, x0y, x0z);
    Eigen::Matrix3d A;

    A << d00, d01, d02, d01, d11, d12, d02, d12, d22;

    Eigen::ArrayXXd pred;
    pred.resize(_rowMax-_rowMin, _colMax-_colMin);

    for (int x = _colMin; x < _colMax; ++x) {
        for (int y = _rowMin; y < _rowMax; ++y) {
            int i = y-_rowMin;
            int j = x-_colMin;

            Eigen::Vector3d v(x, y, frame);
            v -= v0;
            double val = bkg + b0*x + b1*y;
            double arg = -0.5*v.dot(A*v);
            val += c * std::exp(arg);
            pred(i, j) = val;
        }
    }

    return (pred*_maskData[frame-_frameBegin]).matrix();
}

Eigen::MatrixXd PeakFit::chi2(int frame) const
{
    Eigen::ArrayXXd obs = _peakData[frame-_frameBegin];
    Eigen::ArrayXXd pred = predict(frame);
    Eigen::ArrayXXd mask = _maskData[frame-_frameBegin];

    Eigen::ArrayXXd diff = pred-obs;
    return (diff*diff) / (pred+(1.0-mask));
}

double PeakFit::maxIntensity() const
{
    double max = -1000;

    for (int i = 0; i < _frames; ++i) {
        double new_max = _peakData[i].maxCoeff();
        max = std::max(max, new_max);
    }

    return max;
}

Eigen::VectorXd PeakFit::defaultParams() const
{
    Eigen::VectorXd p;
    p.resize(13);

    double bkg = 0.0;
    double nbkg = 0;

    for (int frame = 0; frame < _frames; ++frame) {
        bkg += _peakData[frame].sum();
        nbkg += _maskData[frame].sum();
    }

    bkg /= nbkg;

    Eigen::Vector3d x0 = _peak->getPeak()->getAABBCenter();

    double b0 = 0.0, b1 = 0.0;
    double c = maxIntensity()-bkg;

    double d00, d01, d02, d11, d12, d22;

    // rough for now!!
    Eigen::Vector3d lower = _peak->getPeak()->getLower();
    Eigen::Vector3d upper  =_peak->getPeak()->getUpper();

    auto dx = upper-lower;
    double scale = 1.0 / 3.0;

    d00 = dx(0);
    d11 = dx(1);
    d22 = dx(2);

    d01 = d02 = d12 = 0.0;

    p << bkg, b0, b1, c, d00, d01, d02, d11, d12, d22, x0(0), x0(1), x0(2);
    return p;
}

bool PeakFit::fit(IMinimizer &minimizer)
{
    return false;
}



} // namespace Crystal
} // namespace SX




