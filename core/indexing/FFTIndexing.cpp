//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/indexing/FFTIndexing.cpp
//! @brief     Implements class FFTIndexing
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>
#include <cmath>
#include <complex>
#include <random>
#include <vector>

#include <unsupported/Eigen/FFT>

#include "core/indexing/FFTIndexing.h"
#include "core/geometry/ReciprocalVector.h"
#include "core/utils/Units.h"

namespace nsx {

FFTIndexing::FFTIndexing(int nSubdiv, double amax) : _nSubdiv(nSubdiv), _amax(amax) {}

std::vector<FFTIndexing::tVector> FFTIndexing::findOnSphere(
    const std::vector<ReciprocalVector>& qvects, unsigned int n_vertices,
    unsigned int nsolutions) const
{
    std::vector<double> projs(qvects.size());
    double qMax = 0;

    for (const auto& v : qvects) {
        double norm = v.rowVector().squaredNorm();
        if (norm > qMax)
            qMax = norm;
    }

    qMax = sqrt(qMax); // max norm of all vectors
    size_t nPoints = size_t(
        std::lround(std::ceil(2 * qMax * _nSubdiv * _amax))); // number of points in histogram

    if (nPoints % 2) {
        ++nPoints;
    }

    size_t nPointsHalf = nPoints / 2;
    double dq = 2 * qMax / nPoints;
    double dqInv = 1.0 / dq;

    std::vector<double> hist(nPoints, 0); // reciprocal space histogram
    Eigen::FFT<double> fft; // FFT engine
    std::vector<tVector> result;
    result.reserve(n_vertices);

    // Generate the q direction on Q unit sphere using Fibonacci sphere algorithm
    // See
    // https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    const double rnd(distribution(generator));

    const double offset = 2.0 / n_vertices;

    const double increment = M_PI * (3. - sqrt(5.));

    for (size_t i = 0; i < n_vertices; ++i) {

        const double y = ((i * offset) - 1) + (offset / 2);
        const double r = sqrt(1 - y * y);

        const double phi = fmod(i + rnd, n_vertices) * increment;

        const double x = cos(phi) * r;
        const double z = sin(phi) * r;

        const Eigen::RowVector3d q_direction(x, y, z);
        std::fill(hist.begin(), hist.end(), 0);

        for (const auto& vect : qvects) {
            const Eigen::RowVector3d& q_vector = vect.rowVector();
            double proj = q_vector.dot(q_direction);
            size_t index = size_t((std::floor((proj + qMax) * dqInv)));
            if (index == nPoints)
                --index;

            hist[index] += 1.0;
        }

        std::vector<std::complex<double>> spectrum;

        fft.fwd(spectrum, hist); // Fourier transform the histogram
        double FZero = std::abs(spectrum[0]); // zero mode
        size_t pos_max = 0; // position of maximum mode, other than zero mode
        double value = 0; // value of maxmimum mode

        for (size_t i = size_t(_nSubdiv / 2); i < nPointsHalf; ++i) {
            double current = std::abs(spectrum[i]);

            if (current < 0.7 * FZero)
                continue;

            if (current > value) {
                value = current;
                pos_max = i;
            } else
                break;
        }

        if (pos_max > 2)
            result.push_back(
                tVector(q_direction * (pos_max)*_nSubdiv * _amax / double(nPoints), value));
    }

    std::sort(result.begin(), result.end(), [](const tVector& t1, const tVector& t2) -> bool {
        return (t1.second > t2.second);
    });

    if (nsolutions < result.size())
        result.erase(result.begin() + nsolutions, result.end());

    return result;
}

} // namespace nsx
