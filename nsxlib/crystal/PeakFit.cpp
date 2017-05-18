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
#include "../geometry/IntegrationRegion.h"
#include "../data/IData.h"
#include "../utils/IMinimizer.h"
#include "../utils/Round.h"
#include "../utils/Interpolator.h"

namespace nsx {

PeakFit::PeakFit(sptrPeak3D peak): _peak(peak)
{
    IntegrationRegion region(peak->getShape());
    auto background = region.getBackground();

    auto lower = background.getLower();
    auto upper = background.getUpper();

    _frameBegin = std::max(ifloor(lower(2)), 0);
    _frameEnd = std::min(static_cast<std::size_t>(iceil(upper(2))), _peak->getData()->getNFrames());

    _rowMin = ifloor(lower(1));
    _rowMax = iceil(upper(1));

    _colMin = ifloor(lower(0));
    _colMax = iceil(upper(0));

    const int nrows = _rowMax-_rowMin;
    const int ncols = _colMax-_colMin;

    _frames = _frameEnd - _frameBegin;
    _peakData.resize(static_cast<size_t>(_frames));
    _maskData.resize(static_cast<size_t>(_frames));

    for (unsigned int i = _frameBegin; i < _frameEnd; ++i) {
        auto frameData = _peak->getData()->getFrame(i);
        _peakData[i-_frameBegin] = frameData.block(_rowMin, _colMin, nrows, ncols).cast<double>();
        _maskData[i-_frameBegin].resize(nrows, ncols);

        for (unsigned int r = 0; r < nrows; ++r) {
            for (unsigned int c = 0; c < ncols; ++c) {
                Eigen::Vector4d p(c+_colMin, r+_rowMin, i, 1);
                _maskData[i-_frameBegin](r,c) = background.isInside(p) ? 1.0 : 0.0;
            }
        }

        //_peakData[i-_frameBegin] *= _maskData[i-_frameBegin];
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

    int i = 0;

    for (int idx = 1; idx < 2*_frames-1; ++idx ) {

        double frame = idx / 2.0 + _frameBegin;


        Eigen::ArrayXXd pred = predict(params, frame);
        Eigen::ArrayXXd diff = (pred-peakData(frame)) * mask(frame);


        for (int r = 0; r < _rowMax-_rowMin; ++r) {
            for (int c = 0; c < _colMax-_colMin; ++c) {
                res(i++) = diff(r, c);
                //res(i++) = std::pow(std::abs(diff(r, c)), 0.5);
            }
        }

        // also add difference in integrated intensity
        res(i++) = diff.sum();
    }

    return 0;
}

int PeakFit::numParams() const
{
    return _params.size();
}

int PeakFit::numValues() const
{
    // return (_rowMax-_rowMin)*(_colMax-_colMin)*(_frames);
    return (_rowMax-_rowMin)*(_colMax-_colMin)*(2*_frames-1) + _frames;
}

Eigen::ArrayXXd PeakFit::peakData(double frame) const
{
    return interpolate(_peakData, frame-frameBegin());
}

Eigen::ArrayXXd PeakFit::predict(double frame) const
{
    return predict(_params, frame);
}

Eigen::ArrayXXd PeakFit::background(const Eigen::VectorXd& params, double frame) const
{
    const Eigen::VectorXd& p = params;

    int i = 0;

    double x0x = p(i++);
    double x0y = p(i++);
    double x0z = p(i++);

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

//    double e0 = p(i++);
//    double e1 = p(i++);
//    double e2 = p(i++);
//    double e00 = p(i++);
//    double e01 = p(i++);
//    double e02 = p(i++);
//    double e11 = p(i++);
//    double e12 = p(i++);
//    double e22 = p(i++);

    Eigen::Vector3d v0(x0x, x0y, x0z);
    Eigen::ArrayXXd pred;

    pred.resize(_rowMax-_rowMin, _colMax-_colMin);

    for (int x = _colMin; x < _colMax; ++x) {
        for (int y = _rowMin; y < _rowMax; ++y) {
            int i = y-_rowMin;
            int j = x-_colMin;

            Eigen::Vector3d v(x, y, frame);
            v -= v0;
            double val = bkg + b0*v(0) + b1*v(1);
            pred(i, j) = val;
        }
    }

    const unsigned int idx = integerFrame(frame)-_frameBegin;
    return  pred; // * mask(frame);
}

Eigen::ArrayXXd PeakFit::predict(const Eigen::VectorXd &params, double frame) const
{
    const Eigen::VectorXd& p = params;
    // used for numerical integration
    const int num_points = 20;
    const double dz = 1.0 / (double)num_points;

    int i = 0;

    double x0x = p(i++);
    double x0y = p(i++);
    double x0z = p(i++);

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

//    double e0 = p(i++);
//    double e1 = p(i++);
//    double e2 = p(i++);
//    double e00 = p(i++);
//    double e01 = p(i++);
//    double e02 = p(i++);
//    double e11 = p(i++);
//    double e12 = p(i++);
//    double e22 = p(i++);

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
            double val = bkg + b0*v(0) + b1*v(1);

            const double zmin = frame-0.5-x0z;
            const double zmax = frame+0.5-x0z;

            double sum = 0.0;

//            for (int i = 0; i < num_points; ++i) {
//                v(2) = zmin + i*dz;

//                double arg = -0.5*v.dot(A*v);
//                double gauss = std::exp(arg);

//                sum += c * gauss;
//                sum += e0*v(0)*gauss;
//                sum += e1*v(1)*gauss;
//                sum += e2*v(2)*gauss;

//                sum += e00*v(0)*v(0)*gauss;
//                sum += e01*v(0)*v(1)*gauss;
//                sum += e02*v(0)*v(2)*gauss;
//                sum += e11*v(1)*v(1)*gauss;
//                sum += e12*v(1)*v(2)*gauss;
//                sum += e22*v(2)*v(2)*gauss;
//            }

//            pred(i, j) = val + dz*sum;


            double arg = -0.5*v.dot(A*v);
            double gauss = std::exp(arg);

            sum += c * gauss;
//            sum += e0*v(0)*gauss;
//            sum += e1*v(1)*gauss;
//            sum += e2*v(2)*gauss;

//            sum += e00*v(0)*v(0)*gauss;
//            sum += e01*v(0)*v(1)*gauss;
//            sum += e02*v(0)*v(2)*gauss;
//            sum += e11*v(1)*v(1)*gauss;
//            sum += e12*v(1)*v(2)*gauss;
//            sum += e22*v(2)*v(2)*gauss;


            pred(i, j) = val + sum;
        }
    }

    return pred; // * mask(frame);
}

Eigen::ArrayXXd PeakFit::mask(double frame) const
{
    if (frame < frameBegin())
        frame = frameBegin();

    if ( frame > frameEnd()-1)
        frame = frameEnd()-1;

    int frame0 = ifloor(frame);
    int frame1 = ifloor(frame+1);

    if (frame0 < frameBegin())
        frame0 = frameBegin();

    if (frame1 >= frameEnd())
        frame1 = frameEnd()-1;

    assert(frame0 >= frameBegin());
    assert(frame0 < frameEnd());
    assert(frame1 >= frameBegin());
    assert(frame1 < frameEnd());

    double t = frame-frame0;

    assert(t >= 0);
    assert(t <= 1);

    frame0 -= frameBegin();
    frame1 -= frameBegin();

    return (1-t)*_maskData[frame0] + t*_maskData[frame1];
}

Eigen::ArrayXXd PeakFit::chi2(std::size_t frame) const
{
    Eigen::ArrayXXd obs = _peakData[frame-_frameBegin];
    Eigen::ArrayXXd pred = predict(frame);
    Eigen::ArrayXXd maskf = mask(frame);

    Eigen::ArrayXXd diff = pred-obs;
    return (diff*diff) / (pred+(1.0-maskf));
}

Eigen::ArrayXXd PeakFit::relDifference(std::size_t frame) const
{
    Eigen::ArrayXXd obs = _peakData[frame-_frameBegin];
    Eigen::ArrayXXd pred = predict(frame);
    Eigen::ArrayXXd maskf = mask(frame);

    Eigen::ArrayXXd diff = pred-obs;
    return diff.abs() / (obs+(1.0-maskf));
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
    p.resize(22);

    double bkg = 0.0;
    double nbkg = 0;

    for (int frame = 0; frame < _frames; ++frame) {
        auto msk = mask(frameBegin()+frame);
        bkg += (_peakData[frame]*msk).sum();
        nbkg += msk.sum();
    }

    bkg /= nbkg;

    Eigen::Vector3d x0 = _peak->getShape().getAABBCenter();

    double b0 = 0.0, b1 = 0.0;
    double c = maxIntensity()-bkg;

    double d00, d01, d02, d11, d12, d22;

    // rough for now!!
    auto shape = _peak->getShape();
    Eigen::Vector3d lower = _peak->getShape().getLower();
    Eigen::Vector3d upper  =_peak->getShape().getUpper();

    Eigen::Matrix3d rs = shape.getRSinv();
    Eigen::Matrix3d a = 2.0 * rs.transpose()*rs;

    d00 = a(0, 0);
    d11 = a(1, 1);
    d22 = a(2, 2);
    d01 = a(0, 1);
    d12 = a(1, 2);
    d02 = a(0, 2);

    double e0, e1, e2, e00, e01, e02, e11, e12, e22;
    e0 = e1 = e2 = e00 = e01 = e02 = e11 = e12 = e22 = 0.0;

    p <<  x0(0), x0(1), x0(2), 0.95*bkg, b0, b1, c, d00, d01, d02, d11, d12, d22,
            e0, e1, e2, e00, e01, e02, e11, e12, e22;
    return p;
}

bool PeakFit::fit(IMinimizer& minimizer)
{
    auto min_func = [this](const Eigen::VectorXd& par, Eigen::VectorXd& res) -> int
    {
        residuals(par, res);
        return 0;
    };

    minimizer.initialize(numParams(), numValues());
    minimizer.set_f(min_func);
    minimizer.setParams(_params);

    if (minimizer.fit(200) ) {
        _params = minimizer.params();
        return true;
    }
    return false;
}

int PeakFit::frameBegin() const
{
    return _frameBegin;
}

int PeakFit::frameEnd() const
{
    return _frameEnd;
}

int PeakFit::integerFrame(double f) const
{
    int fint = iround(f);
    if (fint < _frameBegin) fint = _frameBegin;
    if (fint >= _frameEnd) fint = _frameEnd-1;
    return fint;
}

} // end namespace nsx
