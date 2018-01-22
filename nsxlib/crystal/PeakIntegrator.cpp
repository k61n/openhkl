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
#include "PeakIntegrator.h"

namespace nsx {

PeakIntegrator::PeakIntegrator(const IntegrationRegion& region, const DataSet& data):
    _region(region),
    _data_start(),
    _data_end(),
    _start_x(),
    _end_x(),
    _start_y(),
    _end_y(),
    _point1(),
    _dx(),
    _dy()
{

    auto aabb = region.aabb();
    _lower = aabb.lower();
    _upper = aabb.upper();

    const int data_start = std::round((std::ceil(_lower[2])));
    const int data_end = std::round((std::floor(_upper[2])));

    const int start_x = std::round((std::floor(_lower[0])));
    const int end_x = std::round((std::ceil(_upper[0])));

    const int start_y = std::round((std::floor(_lower[1])));
    const int end_y = std::round((std::ceil(_upper[1])));

    _start_x = (start_x > 0 ? start_x : 0);
    _start_y = (start_y > 0 ? start_y : 0);
    _data_start = (data_start > 0 ? data_start : 0);

    _end_x = (end_x > static_cast<int>(data.nCols()-1) ? data.nCols()-1 : end_x);
    _end_y = (end_y > static_cast<int>(data.nRows()-1) ? data.nRows()-1 : end_y);
    _data_end = (data_end > static_cast<int>(data.nFrames()-1) ? data.nFrames()-1 : data_end);

    const int n_frames = _data_end-_data_start + 1;

    // Allocate all vectors
    Eigen::VectorXd zero = Eigen::VectorXd::Zero(n_frames);
    _projection = zero;
    _projectionPeak = zero;
    _projectionBkg = zero;

    _dx = int(_end_x - _start_x)+1;
    _dy = int(_end_y - _start_y)+1;

    _shellIntensity.resize(n_frames, _region.nslices());
    _shellIntensity.setZero();
    _shellPoints.resize(n_frames, _region.nslices());
    _shellPoints.setZero();    
}

void PeakIntegrator::step(const Eigen::MatrixXi& frame, size_t idx, const Eigen::MatrixXi& mask)
{
    if (idx < _data_start || idx > _data_end) {
        return;
    }

    auto _peak_data = frame.block(_start_y, _start_x, _dy,_dx).array().cast<double>();
    auto _peak_mask = Eigen::ArrayXXd(_dy, _dx);
    auto _bkg_mask = Eigen::ArrayXXd(_dy, _dx);

    _peak_mask.setZero();
    _bkg_mask.setZero();

    _projection[idx-_data_start] += _peak_data.sum();

    for (unsigned int x = _start_x; x <= _end_x; ++x) {
        for (unsigned int y = _start_y; y <= _end_y; ++y) {
            int intensity = frame(y, x);

            _point1 << x, y, idx;
            const int slice = _region.classifySlice(_point1);

            const bool inpeak = (slice > 0);
            const bool inbackground = (slice == 0) && (mask(y, x) <= 0);

            if (slice >= 0) {
                _shellIntensity(idx-_data_start, slice) += intensity;
                _shellPoints(idx-_data_start, slice) += 1;
            }
        }
    }   
}

void PeakIntegrator::end()
{
    // get average background
    auto background = getMeanBackground();
    const double avgBkg = background.value();
    const double stdBkg = background.sigma();
    const double scale = stdBkg*stdBkg / avgBkg;
    const double nbkg = pointsBkg().sum();
    

    // find the shell with best sigma/I
    size_t best_slice = _region.nslices()-1;
    double best_ratio = 1e10;
    double npoints = 0;
    double I = 0;
    
    // determine the slice that minimizes sigma(I) / I
    for (size_t slice = 0; slice < _region.nslices(); ++slice) {
        for (size_t idx = _data_start; idx <= _data_end; ++idx) {
            npoints += _shellPoints(idx-_data_start, slice);
            I += _shellIntensity(idx-_data_start, slice);
        }
        if (slice == 0) {
            continue;
        }
        const double sigma = std::sqrt(scale*I + npoints*npoints*stdBkg*stdBkg / nbkg);
        const double inten = I - npoints*avgBkg;
        const double ratio = sigma / inten;
        if (std::isfinite(ratio) && ratio < best_ratio) {
            best_slice = slice;
            best_ratio = ratio;
        }
    }

    _region.setBestSlice(best_slice);
   
    #if 0
    // OLD CODE BELOW

 

    // subtract background from peak
    _projectionPeak = _countsPeak - avgBkg*_pointsPeak;

    // note: this "background" simply refers to anything in the AABB but NOT in the peak
    _projectionBkg = _projection-_projectionPeak;

    // testing

    auto&& aabb = _region.aabb();
    auto lb = aabb.lower();
    auto ub = aabb.upper();

    double npx = (ub[0]-lb[0])*(ub[1]-lb[1]);

    for (auto i = 0; i < _projectionBkg.size(); ++i) {
        if (_pointsBkg(i) > 0.1) {
            _projectionBkg(i) = _countsBkg(i) / _pointsBkg(i) * npx;
        }
        else {
            _projectionBkg(i) = avgBkg * npx;
        }
        _projection(i) = _projectionPeak(i) + _projectionBkg(i);
    }
    #endif
}

const Eigen::ArrayXd& PeakIntegrator::getProjectionPeak() const
{
    return _projectionPeak;
}

const Eigen::ArrayXd& PeakIntegrator::getProjectionBackground() const
{
    return _projectionBkg;
}

const Eigen::ArrayXd& PeakIntegrator::getProjection() const
{
    return _projection;
}

const Eigen::ArrayXd PeakIntegrator::getPeakError() const
{
    auto background = getMeanBackground();

    const double mean_bkg = background.value();
    const double std_bkg = background.sigma();
    const double nbkg = pointsBkg().sum();
    const double scale = std_bkg * std_bkg / mean_bkg;
    const auto& npeak = pointsPeak();

    Eigen::ArrayXd error2 = scale * countsPeak() + npeak*npeak * mean_bkg / nbkg;
    return error2.sqrt();
}

Intensity PeakIntegrator::getMeanBackground() const
{   
    auto&& counts = countsBkg();
    auto&& points = pointsBkg();
    const double nbkg = points.sum();
    const double mean = counts.sum() / nbkg;
    auto&& deviation = counts-mean*points;
    const double variance = (deviation*deviation).sum() / (nbkg-1);

    return Intensity(mean, variance);
}

const IntegrationRegion& PeakIntegrator::getRegion() const
{
    return _region;
}

Intensity PeakIntegrator::getPeakIntensity() const
{
    auto background = getMeanBackground();

    const double mean_bkg = background.value();
    const double std_bkg = background.sigma();
    const double nbkg = pointsBkg().sum();
    const double npeak = pointsPeak().sum();
    const double scale = std_bkg * std_bkg / mean_bkg;
    const double total = countsPeak().sum();

    const double error2 = scale*total + npeak*npeak*std_bkg*std_bkg/nbkg;

    return Intensity(total - npeak*mean_bkg, error2);
}

// TODO
double PeakIntegrator::pValue() const
{
    #if 0
    // assumption: all background pixel counts are Poisson processes with rate R
    // therefore, we can estimate the rate R as below:
    const double R = _countsBkg.sum() / _pointsBkg.sum();

    // R should always be positive!
    if (std::isnan(R) || R <= 0.0) {
        return 1.0;
    }

    const auto points_peak = _pointsPeak.sum();

    // integration region too small: no points in peak!
    // todo: threshold should not be arbitrary
    if (points_peak < 10) {
        return 1.0;
    }

    // null hypothesis: the pixels inside the peak are Poisson processes with rate R
    // therefore, by central limit theorem the average value
    const double avg = _countsPeak.sum() / points_peak;
    // is normal with mean R and variance:
    const double var = R / points_peak;

    // avg should be finite and positive
    if (std::isnan(avg) || avg <= 0.0) {
        return 1.0;
    }

    // variance should be finite and positive
    if (std::isnan(var) || var <= 0.0) {
        return 1.0;
    }

    // thus we obtain the following standard normal variable
    //const double z = (avg-R) / std::sqrt(var);
    const double z = (avg-R) / (getMeanBackground().sigma() / points_peak);

    // compute the p value
    //const double p = 1.0 - 0.5 * (1.0 + std::erf(z / std::sqrt(2)));
    const double p = 0.5 - 0.5 * (std::erf(z / std::sqrt(2)));

    return p;
    #endif

    return 0.0;
}

Eigen::ArrayXd PeakIntegrator::countsPeak() const
{
    auto best_slice = _region.bestSlice();
    Eigen::ArrayXd counts(_data_end-_data_start+1);
    counts.setZero();
    // update the intensity/counts based on optimal slice
    for (size_t idx = _data_start; idx <= _data_end; ++idx) {
        for (size_t slice = 1; slice <= best_slice; ++slice) {
            counts(idx-_data_start) += _shellIntensity(idx-_data_start, slice);
        }
    }
    return counts;
}

Eigen::ArrayXd PeakIntegrator::countsBkg() const
{
    auto best_slice = _region.bestSlice();
    Eigen::ArrayXd counts(_data_end-_data_start+1);
    counts.setZero();

    for (size_t idx = _data_start; idx <= _data_end; ++idx) {
        counts(idx-_data_start) += _shellIntensity(idx-_data_start, 0);
    }
    return counts;
}

Eigen::ArrayXd PeakIntegrator::pointsPeak() const
{
    auto best_slice = _region.bestSlice();
    Eigen::ArrayXd points(_data_end-_data_start+1);
    points.setZero();
    // update the intensity/counts based on optimal slice
    for (size_t idx = _data_start; idx <= _data_end; ++idx) {
        for (size_t slice = 1; slice <= best_slice; ++slice) {
            points(idx-_data_start) += _shellPoints(idx-_data_start, slice);
        }
    }
    return points;
}

Eigen::ArrayXd PeakIntegrator::pointsBkg() const
{
    auto best_slice = _region.bestSlice();
    Eigen::ArrayXd points(_data_end-_data_start+1);
    points.setZero();

    for (size_t idx = _data_start; idx <= _data_end; ++idx) {
        points(idx-_data_start) += _shellPoints(idx-_data_start, 0);
    }
    return points;
}

} // end namespace nsx
