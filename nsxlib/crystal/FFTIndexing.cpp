#include <algorithm>
#include <cmath>
#include <complex>
#include <vector>

#include <unsupported/Eigen/FFT>

#include "FFTIndexing.h"
#include "Units.h"

namespace nsx {

FFTIndexing::FFTIndexing(int nSubdiv,double amax) : _nSubdiv(nSubdiv), _amax(amax)
{
}

std::vector<tVector> FFTIndexing::findOnSphere(const std::vector<Eigen::RowVector3d>& qvects, unsigned int nstacks, unsigned int nsolutions) const
{
    std::vector<double> projs(qvects.size());
    double qMax = 0;

    for (const auto& v : qvects) {
        double norm = v.squaredNorm();
        if (norm > qMax) {
            qMax = norm;
        }
    }

    qMax = sqrt(qMax); // max norm of all vectors
    size_t nPoints = size_t(std::lround(std::ceil(2*qMax*_nSubdiv*_amax))); // number of points in histogram

    if (nPoints%2) {
        ++nPoints;
    }

    size_t nPointsHalf = nPoints / 2;
    double dq = 2*qMax / nPoints;
    double dqInv = 1.0 / dq;
    double twopi = 2.0*M_PI;
    double fact1 = 0.5*M_PI / nstacks;
    double fact2 = twopi * nstacks;

    std::vector<double> hist(nPoints, 0); // reciprocal space histogram
    Eigen::FFT<double> fft;              // FFT engine
    std::vector<tVector> result;
    result.reserve(nstacks*nstacks);

    for (unsigned int th = 0; th <= nstacks; ++th) {
        double theta = th*fact1;
        double ctheta = cos(theta);
        double stheta = sin(theta);
        int nslices = int(std::lround(fact2*stheta + 1));

        for (int ph = 0; ph < nslices; ++ph) {
            double phi = ph * twopi/ double(nslices);
            double sp = sin(phi);
            double cp = cos(phi);
            const Eigen::Vector3d N(stheta*cp, stheta*sp, ctheta);
            std::fill(hist.begin(), hist.end(), 0);

            for (const auto& vect: qvects) {
                double proj = vect.dot(N);
                size_t index = size_t((std::floor((proj+qMax)*dqInv)));
                if (index == nPoints)
                    --index;

                hist[index] += 1.0;
            }

            std::vector<std::complex<double>> spectrum;

            fft.fwd(spectrum, hist); // Fourier transform the histogram
            double FZero = std::abs(spectrum[0]); // zero mode
            size_t pos_max = 0; // position of maximum mode, other than zero mode
            double value = 0; // value of maxmimum mode

            for (size_t i = size_t(_nSubdiv/2); i < nPointsHalf; ++i) {
                double current = std::abs(spectrum[i]);

                if (current < 0.7*FZero)
                    continue;

                if (current > value) {
                    value = current;
                    pos_max = i;
                }
                else
                    break;
            }

            if (pos_max > 2)
                result.push_back(tVector(N*(pos_max)*_nSubdiv*_amax/double(nPoints), value));
        }
    }

    std::sort(result.begin(), result.end(),
              [](const tVector& t1, const tVector& t2)->bool
                { return (t1._quality > t2._quality); });

    if (nsolutions < result.size())
        result.erase(result.begin() + nsolutions, result.end());

    return result;
}

} // end namespace nsx
