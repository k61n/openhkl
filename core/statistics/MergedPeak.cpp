//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/statistics/MergedPeak.cpp
//! @brief     Implements class MergedPeak
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

#include <gsl/gsl_cdf.h>

#include "base/geometry/ReciprocalVector.h"
#include "base/utils/Random.h"
#include "core/data/DataSet.h"
#include "core/peak/Peak3D.h"
#include "core/statistics/MergedPeak.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"

namespace ohkl {

MergedPeak::MergedPeak(const SpaceGroup& grp, bool sum_intensity, bool friedel)
    : _intensity(0.0, 0.0), _grp(grp), _sum_intensity(sum_intensity), _friedel(friedel)
{
}

MergeFlag MergedPeak::addPeak(Peak3D* peak)
{
    if (_sum_intensity) {
        if (peak->sumRejectionFlag() != RejectionFlag::NotRejected)
            return MergeFlag::Invalid;
    } else {
        if (peak->profileRejectionFlag() != RejectionFlag::NotRejected)
            return MergeFlag::Invalid;
    }

    const UnitCell* cell = peak->unitCell();
    const ReciprocalVector q = peak->q();

    if (_peaks.empty()) {
        _hkl = MillerIndex(q, *cell);
        determineRepresentativeHKL();
    } else {
        MillerIndex hkl(q, *cell);
        if (!_grp.isEquivalent(_hkl, hkl, _friedel))
            return MergeFlag::Inequivalent;
    }
    // add peak to list
    _peaks.push_back(peak);
    if (_sum_intensity)
        _intensity += peak->correctedSumIntensity();
    else
        _intensity += peak->correctedProfileIntensity();
    return MergeFlag::Added;
}

MillerIndex MergedPeak::index() const
{
    return _hkl;
}

Intensity MergedPeak::intensity() const
{
    return _intensity / _peaks.size();
}

size_t MergedPeak::redundancy() const
{
    return _peaks.size();
}

//! The representative of the equivalences is defined as the one whose h, k and l
//! are maximum E.g. the representative of
//! (2,1,2),(1,-3,5),(-2,4,3),(4,0,5),(7,8-2),(2,6,-1) will be (7,8-2)
void MergedPeak::determineRepresentativeHKL()
{
    Eigen::RowVector3d best_hkl = _hkl.rowVector().cast<double>();
    std::vector<Eigen::RowVector3d> equivs;

    for (const auto& g : _grp.groupElements()) {
        const Eigen::Matrix3d rotation = g.getRotationPart().transpose();
        equivs.emplace_back(best_hkl * rotation);

        if (_friedel)
            equivs.emplace_back(-best_hkl * rotation);
    }

    auto compare_fn = [=](const Eigen::RowVector3d& a, const Eigen::RowVector3d& b) -> bool {
        const double eps = 1e-5;
        for (auto i = 0; i < 3; ++i) {
            if (std::abs(a(i) - b(i)) > eps)
                return a(i) > b(i);
        }
        return false;
    };

    best_hkl = *std::min_element(equivs.begin(), equivs.end(), compare_fn);

    for (int i = 0; i < 3; ++i)
        _hkl(i) = int(std::lround(best_hkl(i)));
}

std::vector<Peak3D*> MergedPeak::peaks() const
{
    return _peaks;
}

std::pair<MergedPeak, MergedPeak> MergedPeak::split(bool sum_intensity) const
{
    // make copy of peak list
    std::vector<size_t> random_idx(_peaks.size());

    for (unsigned int j = 0; j < _peaks.size(); ++j)
        random_idx[j] = j;

    // randomly reorder
    std::shuffle(random_idx.begin(), random_idx.end(), Random::getRNG());

    unsigned int parity = Random::intRange() % 2;

    MergedPeak p1(_grp, sum_intensity, _friedel), p2(_grp, sum_intensity, _friedel);

    for (unsigned int i = 0; i < _peaks.size(); ++i) {
        auto idx = random_idx[i];
        auto p = _peaks[idx];

        if ((i % 2) == parity)
            p1.addPeak(p);
        else
            p2.addPeak(p);
    }
    return std::make_pair(p1, p2);
}

bool operator<(const MergedPeak& p, const MergedPeak& q)
{
    const auto& a = p.index();
    const auto& b = q.index();

    if (a(0) != b(0))
        return a(0) < b(0);
    if (a(1) != b(1))
        return a(1) < b(1);
    return a(2) < b(2);
}

//! The merged intensity and error are computed as \f$I_{\mathrm{merge}} =
//! N^{-1} \sum_i I_i\f$, \f$\sigma_{\mathrm{merge}}^2 = N^{-2}\sum_i
//! \sigma_i^2\f$. where \f$N\f$ is the redundancy of the reflection. This
//! method computes the statistic \f[ \chi^2 = \frac{\sum_i
//! (I_i-I_{\mathrm{merge}})^2}{N \sigma_{\mathrm{merge}}^2}, \f] which is
//! approximately a chi-squared statistic with \f$N-1\f$ degrees of freedom.
double MergedPeak::chi2() const
{
    double I_merge = intensity().value();

    if (redundancy() < 1.99) // if there is no redundancy, we cannot compute chi2
        return 0;

    double chi_sq = 0;
    for (const auto& peak : _peaks) {
        Intensity I;
        if (_sum_intensity)
            I = peak->correctedSumIntensity();
        else
            I = peak->correctedProfileIntensity();
        const double std = I.sigma();
        const double x = (I.value() - I_merge) / (std * std);
        chi_sq += x * x;
    }
    return chi_sq;
}

//! This method returns the probability that a chi-squared random variable takes
//! a value less than the computed value of MergedPeak::chi2(). A large p-value
//! indicates that the computed variance is larger than the expected variance,
//! indicating the possibility of a systematic error either in the integrated
//! intensities or in the computed error.
double MergedPeak::pValue() const
{
    // todo: k or k-1?? need to check
    const double k = redundancy() - 1.0;

    // if there is only one observation, we cannot compute a p-value
    if (k < 0.9)
        return 0;

    const double x = chi2();
    return gsl_cdf_chisq_P(x, k);
}

} // namespace ohkl
