//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/FFTIndexing.cpp
//! @brief     Implements function findOnSphere
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>
#include <chrono>
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <random>
#include <vector>

#include <unsupported/Eigen/FFT>

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Units.h"
#include "core/algo/FFTIndexing.h"

//#define RANDOMISE_FFTINDEXING_SPHERE
//#define DEBUG_HISTOGRAMS


namespace nsx {

//! Returns a list of approximately equal distributed points on the unit sphere.
//!
//! Only used by algo::findOnSphere, but exposed globally to allow for TestPointsOnSphere.

std::vector<Eigen::RowVector3d> algo::pointsOnSphere(unsigned int n_vertices)
{
    std::vector<Eigen::RowVector3d> result;
    result.reserve(n_vertices);

#ifdef RANDOMISE_FFTINDEXING_SPHERE
    // random number generator, seeded with ms since epoch
    static auto epoch = std::chrono::system_clock::now().time_since_epoch();
    static std::mt19937 generator(
        std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count());
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    const double rnd(distribution(generator));
#else
    // TODO: TestAutoIndexer.py only works with this special number, something is wrong...
    const double rnd(0.131538);
#endif


    // We use the Fibonacci sphere algorithm, which is simple, and just good enough.
    // See https://stackoverflow.com/questions/9600801/evenly-distributing-n-points-on-a-sphere.
    const double offset = 2.0 / n_vertices;
    const double increment = M_PI * (3. - sqrt(5.));

    for (size_t i = 0; i < n_vertices; ++i) {
        const double y = ((i * offset) - 1) + (offset / 2);
        const double r = sqrt(1 - y * y);
        const double phi = fmod(i + rnd, n_vertices) * increment;
        const double x = cos(phi) * r;
        const double z = sin(phi) * r;

        result.push_back({x, y, z});
    }

    return result;
}


#ifdef DEBUG_HISTOGRAMS
// Output a histogram and its trafo for debugging.
// Plot in gnuplot with:
//      plot "hist_0.dat" u 4:5 w boxes
//      plot "hist_0.dat" u 4:6 w boxes
static void save_histogram(
    std::size_t projidx, const Eigen::RowVector3d& proj_dir, double dq, double qMax,
    const std::vector<double>& hist, const std::vector<std::complex<double>>& hist_ft)
{
    int spacing = 16;
    std::ofstream ofHist("hist_" + std::to_string(projidx) + ".dat");

    ofHist << std::setw(spacing) << std::left << "# proj_dir_x"
           << " " << std::setw(spacing) << std::left << "proj_dir_y"
           << " " << std::setw(spacing) << std::left << "proj_dir_z"
           << " " << std::setw(spacing) << std::left << "q_proj"
           << " " << std::setw(spacing) << std::left << "hist"
           << " " << std::setw(spacing) << std::left << "hist_ft"
           << "\n";

    for (std::size_t idx = 0; idx < std::min(hist.size(), hist_ft.size()); ++idx) {
        ofHist << std::setw(spacing) << std::left << proj_dir[0] << " " // proj_dir[0]
               << std::setw(spacing) << std::left << proj_dir[1] << " " // proj_dir[1]
               << std::setw(spacing) << std::left << proj_dir[2] << " " // proj_dir[2]
               << std::setw(spacing) << std::left << double(idx) * dq - qMax << " " // q_proj
               << std::setw(spacing) << std::left << hist[idx] << " " << std::setw(spacing)
               << std::left << std::abs(hist_ft[idx]) << "\n";
    }

    ofHist.flush();
}
#endif


std::vector<Eigen::RowVector3d> algo::findOnSphere(
    const std::vector<ReciprocalVector>& qvects, unsigned int n_vertices, unsigned int nsolutions,
    int nSubdiv, double amax, double freq_tol)
{
    std::vector<double> projs(qvects.size());

    // compute maximum length of input vectors
    double qMax = 0;
    for (const auto& v : qvects) {
        double norm = v.rowVector().squaredNorm();
        if (norm > qMax)
            qMax = norm;
    }
    qMax = sqrt(qMax);

    // set number of points in histogram
    size_t nPoints = size_t(std::lround(std::ceil(2 * qMax * nSubdiv * amax)));
    if (nPoints % 2)
        ++nPoints;
    size_t nPointsHalf = nPoints / 2;

    double dq = 2 * qMax / nPoints;
    double dqInv = 1.0 / dq;

    Eigen::FFT<double> fft; // FFT engine

    std::vector<std::pair<Eigen::RowVector3d, double>> vectorWithQuality;
    vectorWithQuality.reserve(n_vertices);

    std::vector<Eigen::RowVector3d> q_directions = pointsOnSphere(n_vertices);
    for (std::size_t q_diridx = 0; q_diridx < q_directions.size(); ++q_diridx) {
        const Eigen::RowVector3d& q_direction = q_directions[q_diridx];

        std::vector<double> hist(nPoints, 0); // reciprocal space histogram
        for (const auto& vect : qvects) {
            const Eigen::RowVector3d& q_vector = vect.rowVector();
            double proj = q_vector.dot(q_direction);
            size_t index = size_t((std::floor((proj + qMax) * dqInv)));
            if (index == nPoints)
                --index;
            hist[index] += 1;
        }

        std::vector<std::complex<double>> spectrum;
        fft.fwd(spectrum, hist); // Fourier transform the histogram
#ifdef DEBUG_HISTOGRAMS
        save_histogram(q_diridx, q_direction, dq, qMax, hist, spectrum);
#endif

        const double FZero = std::abs(spectrum[0]); // zero mode
        size_t pos_max = 0; // position of maximum mode, other than zero mode
        double value = 0; // value of maxmimum mode

        for (size_t i = size_t(nSubdiv / 2); i < nPointsHalf; ++i) {
            const double current = std::abs(spectrum[i]);

            if (current < freq_tol * FZero)
                continue;
            if (current <= value)
                break;

            value = current;
            pos_max = i;
        }

        if (pos_max > 2)
            vectorWithQuality.push_back(
                {q_direction * double(pos_max * nSubdiv) * amax / double(nPoints), value});
    }

    std::sort(
        vectorWithQuality.begin(), vectorWithQuality.end(),
        [](const auto& t1, const auto& t2) -> bool { return (t1.second > t2.second); });

    if (nsolutions < vectorWithQuality.size())
        vectorWithQuality.erase(vectorWithQuality.begin() + nsolutions, vectorWithQuality.end());

    std::vector<Eigen::RowVector3d> result;
    result.reserve(vectorWithQuality.size());
    for (const auto& pair : vectorWithQuality)
        result.push_back(pair.first);

    return result;
}

} // namespace nsx
