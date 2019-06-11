//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/algo/UserDefinedIndexer.cpp
//! @brief     Implements class UserDefinedUnitCellIndexer
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>

#include <Eigen/QR>

#include "core/algo/UserDefinedIndexer.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"
#include "base/fit/FitParameters.h"
#include "base/fit/Minimizer.h"
#include "base/geometry/ReciprocalVector.h"
#include "base/logger/Logger.h"
#include "core/peak/Peak3D.h"
#include "core/merge_and_filter/PeakFilter.h"

namespace nsx {

using indexer_solution = std::pair<sptrUnitCell, double>;

void UserDefinedUnitCellIndexerParameters::checkParameters() const
{
    if (wavelength <= 0.0)
        throw std::runtime_error("Invalid wavelength. Must be > 0.");

    if (a <= 0.0)
        throw std::runtime_error("Invalid a value. Must be > 0.");

    if (b <= 0.0)
        throw std::runtime_error("Invalid b value. Must be > 0.");

    if (c <= 0.0)
        throw std::runtime_error("Invalid c value. Must be > 0.");

    if (std::fabs(niggli_tolerance - 1.0) > 1.0)
        throw std::runtime_error("Invalid Niggli tolerance. Must be in [0,1].");

    if (std::fabs(gruber_tolerance - 1.0) > 1.0)
        throw std::runtime_error("Invalid Gruber tolerance. Must be in [0,1].");

    if (std::fabs(indexing_tolerance - 1.0) > 1.0)
        throw std::runtime_error("Invalid indexing tolerance. Must be in [0,1].");

    if (std::fabs(indexing_threshold - 1.0) > 1.0)
        throw std::runtime_error("Invalid indexing tolerance. Must be in [0,1].");

    if (n_solutions < 1)
        throw std::runtime_error("Invalid number of solutions. Must be >= 1.");

    if (max_n_q_vectors < 1)
        throw std::runtime_error("Invalid number of q vectors. Must be in [10,500].");
}

UserDefinedUnitCellIndexer::UserDefinedUnitCellIndexer() : _parameters(), _solutions() {}

UserDefinedUnitCellIndexer::UserDefinedUnitCellIndexer(
    const UserDefinedUnitCellIndexerParameters& parameters)
    : UserDefinedUnitCellIndexer()
{
    setParameters(parameters);
}

const UserDefinedUnitCellIndexerParameters& UserDefinedUnitCellIndexer::parameters() const
{
    return _parameters;
}

void UserDefinedUnitCellIndexer::setParameters(
    const UserDefinedUnitCellIndexerParameters& parameters)
{
    _parameters = parameters;
}

void UserDefinedUnitCellIndexer::setPeaks(const PeakList& peaks)
{
    _peaks = peaks;
}

void UserDefinedUnitCellIndexer::index()
{
    PeakFilter peak_filter;
    auto filtered_peaks = peak_filter.enabled(_peaks, true);

    std::multimap<double, Eigen::RowVector3d> q_vectors_mmap;
    for (auto peak : filtered_peaks) {
        Eigen::RowVector3d row_vect = peak->q().rowVector();
        q_vectors_mmap.emplace(row_vect.norm(), row_vect);
    }

    // Compute the dmin and dmax form the input q vectors
    double dmin(std::numeric_limits<double>::infinity());
    double dmax(-std::numeric_limits<double>::infinity());
    for (auto&& p : q_vectors_mmap) {
        const double d = 1.0 / p.first;
        dmin = std::min(d, dmin);
        dmax = std::max(d, dmax);
    }

    // Add little tolerance on dmin and dmax for not missing any q-vectors within
    // [dmin,dmax]
    dmin -= 1.0e-6;
    dmax += 1.0e-6;

    nsx::UnitCell unit_cell(
        _parameters.a, _parameters.b, _parameters.c, _parameters.alpha, _parameters.beta,
        _parameters.gamma);

    const double wavelength = _parameters.wavelength;

    // Generate the q vectors from the initial B matrix within [dmin,dmax]
    // The generation uses the initial B matrix built from the unit cell
    // parameters provided by the user hence without orientation matrix
    auto&& b_mat = unit_cell.reciprocalBasis();
    std::vector<std::pair<Eigen::RowVector3d, Eigen::RowVector3d>> predicted_q_vectors;
    auto hkls = unit_cell.generateReflectionsInShell(dmin, dmax, wavelength);
    for (auto hkl : hkls) {
        Eigen::RowVector3d q = hkl.rowVector().cast<double>() * b_mat;
        predicted_q_vectors.emplace_back(hkl.rowVector().cast<double>(), q);
    }

    using row_vector_pair = std::pair<Eigen::RowVector3d, Eigen::RowVector3d>;

    std::sort(
        predicted_q_vectors.begin(), predicted_q_vectors.end(),
        [](const row_vector_pair& v1, const row_vector_pair& v2) {
            return v1.second.norm() < v2.second.norm();
        });

    if (_parameters.max_n_q_vectors < predicted_q_vectors.size())
        predicted_q_vectors.resize(_parameters.max_n_q_vectors);

    _solutions.clear();
    _solutions.shrink_to_fit();
    _solutions.reserve(_parameters.n_solutions);

    Eigen::Matrix3d b_triplet;

    Eigen::Matrix3d matching_hkls;
    Eigen::Matrix3d matching_q_vectors;

    const double distance_tolerance = _parameters.distance_tolerance;
    const double angular_tolerance = _parameters.angular_tolerance;

    const double volume_tolerance = distance_tolerance * distance_tolerance * distance_tolerance;

    // Triple loop over the predicted q vectors to build predicted q triplet (aka
    // qp)
    for (size_t i = 0; i < predicted_q_vectors.size() - 2; ++i) {

        b_triplet.row(0) = predicted_q_vectors[i].second;

        const double qi_norm = b_triplet.row(0).norm();
        auto lit_i = q_vectors_mmap.lower_bound(qi_norm * (1.0 - distance_tolerance));
        auto uit_i = q_vectors_mmap.upper_bound(qi_norm * (1.0 + distance_tolerance));

        for (size_t j = i + 1; j < predicted_q_vectors.size() - 1; ++j) {

            b_triplet.row(1) = predicted_q_vectors[j].second;

            const double qj_norm = b_triplet.row(1).norm();
            auto lit_j = q_vectors_mmap.lower_bound(qj_norm * (1.0 - distance_tolerance));
            auto uit_j = q_vectors_mmap.upper_bound(qj_norm * (1.0 + distance_tolerance));

            for (size_t k = j + 1; k < predicted_q_vectors.size(); ++k) {

                b_triplet.row(2) = predicted_q_vectors[k].second;

                const double qk_norm = b_triplet.row(2).norm();
                auto lit_k = q_vectors_mmap.lower_bound(qk_norm * (1.0 - distance_tolerance));
                auto uit_k = q_vectors_mmap.upper_bound(qk_norm * (1.0 + distance_tolerance));

                double det_b_triplet = b_triplet.determinant();
                if (det_b_triplet < 1.0e-6)
                    continue;

                std::array<size_t, 3> indexes {{i, j, k}};

                Eigen::Matrix3d bu_triplet;

                // Triple loop over the experimental q vectors to build experimental q
                // triplets which will be match over the predicted q-triplet (aka qe)
                for (auto it_i = lit_i; it_i != uit_i; ++it_i) {

                    bu_triplet.row(0) = it_i->second;
                    ;

                    for (auto it_j = lit_j; it_j != uit_j; ++it_j) {

                        bu_triplet.row(1) = it_j->second;

                        for (auto it_k = lit_k; it_k != uit_k; ++it_k) {

                            bu_triplet.row(2) = it_k->second;

                            double det_bu_triplet = bu_triplet.determinant();
                            if (det_bu_triplet < 1.0e-6)
                                continue;

                            // The two triplets does not match in volume, skip
                            if (std::fabs((det_bu_triplet - det_b_triplet) / det_b_triplet)
                                > volume_tolerance) {
                                continue;
                            }

                            for (size_t ti = 0; ti < 3; ++ti) {

                                const double q1i_norm = b_triplet.row(ti).norm();

                                for (size_t tj = 0; tj < 3; ++tj) {

                                    if (ti == tj)
                                        continue;

                                    const double q1j_norm = b_triplet.row(tj).norm();

                                    for (size_t tk = 0; tk < 3; ++tk) {

                                        if (ti == tk || tj == tk)
                                            continue;

                                        const double q1k_norm = b_triplet.row(tk).norm();

                                        for (size_t tii = 0; tii < 3; ++tii) {

                                            const double q2i_norm = bu_triplet.row(tii).norm();

                                            // Try to match triplet1 and triplet2 i vectors, if the
                                            // match fails skip this experimental triplet
                                            if (std::fabs((q2i_norm - q1i_norm) / q1i_norm)
                                                > distance_tolerance) {
                                                continue;
                                            }

                                            for (size_t tjj = 0; tjj < 3; ++tjj) {

                                                if (tii == tjj)
                                                    continue;

                                                const double q2j_norm = bu_triplet.row(tjj).norm();

                                                // Try to match triplet1 and triplet2 j vectors, if
                                                // the match fails skip this experimental triplet
                                                if (std::fabs((q2j_norm - q1j_norm) / q1j_norm)
                                                    > distance_tolerance) {
                                                    continue;
                                                }

                                                for (size_t tkk = 0; tkk < 3; ++tkk) {

                                                    if (tii == tkk || tjj == tkk)
                                                        continue;

                                                    const double q2k_norm =
                                                        bu_triplet.row(tkk).norm();

                                                    // Try to match triplet1 and triplet2 k vectors,
                                                    // if the match fails skip this experimental
                                                    // triplet
                                                    if (std::fabs((q2k_norm - q1k_norm) / q1k_norm)
                                                        > distance_tolerance) {
                                                        continue;
                                                    }

                                                    const double q1iq1j_angle = std::acos(
                                                        b_triplet.row(ti).dot(b_triplet.row(tj))
                                                        / q1i_norm / q1j_norm);
                                                    const double q2iq2j_angle = std::acos(
                                                        bu_triplet.row(tii).dot(bu_triplet.row(tjj))
                                                        / q2i_norm / q2j_norm);
                                                    if (std::fabs(
                                                            (q2iq2j_angle - q1iq1j_angle)
                                                            / q1iq1j_angle)
                                                        > angular_tolerance) {
                                                        continue;
                                                    }

                                                    const double q1iq1k_angle = std::acos(
                                                        b_triplet.row(ti).dot(b_triplet.row(tk))
                                                        / q1i_norm / q1k_norm);
                                                    const double q2iq2k_angle = std::acos(
                                                        bu_triplet.row(tii).dot(bu_triplet.row(tkk))
                                                        / q2i_norm / q2k_norm);
                                                    if (std::fabs(
                                                            (q2iq2k_angle - q1iq1k_angle)
                                                            / q1iq1k_angle)
                                                        > angular_tolerance) {
                                                        continue;
                                                    }

                                                    const double q1jq1k_angle = std::acos(
                                                        b_triplet.row(tj).dot(b_triplet.row(tk))
                                                        / q1j_norm / q1k_norm);
                                                    const double q2jq2k_angle = std::acos(
                                                        bu_triplet.row(tjj).dot(bu_triplet.row(tkk))
                                                        / q2j_norm / q2k_norm);
                                                    if (std::fabs(
                                                            (q2jq2k_angle - q1jq1k_angle)
                                                            / q1jq1k_angle)
                                                        > angular_tolerance) {
                                                        continue;
                                                    }

                                                    size_t m_i = indexes[ti];
                                                    size_t m_j = indexes[tj];
                                                    size_t m_k = indexes[tk];

                                                    matching_hkls.row(0) =
                                                        predicted_q_vectors[m_i].first;
                                                    matching_hkls.row(1) =
                                                        predicted_q_vectors[m_j].first;
                                                    matching_hkls.row(2) =
                                                        predicted_q_vectors[m_k].first;

                                                    matching_q_vectors.row(0) = bu_triplet.row(tii);
                                                    matching_q_vectors.row(1) = bu_triplet.row(tjj);
                                                    matching_q_vectors.row(2) = bu_triplet.row(tkk);

                                                    auto BtUt = matching_hkls.inverse()
                                                        * matching_q_vectors;

                                                    if (BtUt.determinant() < 0)
                                                        continue;

                                                    auto unit_cell = std::shared_ptr<UnitCell>(
                                                        new UnitCell(BtUt, true));

                                                    unit_cell->setIndexingTolerance(
                                                        _parameters.indexing_tolerance);

                                                    double n_indexed_q_vectors(0);
                                                    for (auto q : q_vectors_mmap) {
                                                        MillerIndex miller_index(
                                                            nsx::ReciprocalVector(q.second),
                                                            *unit_cell);
                                                        if (miller_index.indexed(
                                                                unit_cell->indexingTolerance())) {
                                                            ++n_indexed_q_vectors;
                                                        }
                                                    }

                                                    const double quality =
                                                        n_indexed_q_vectors
                                                        / static_cast<double>(
                                                              q_vectors_mmap.size());
                                                    if (quality < _parameters.indexing_threshold)
                                                        continue;

                                                    // The predicted and experimental triplets
                                                    // match, keep that solution
                                                    _solutions.emplace_back(unit_cell, quality);
                                                    if (_solutions.size()
                                                        >= _parameters.n_solutions) {
                                                        return;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void UserDefinedUnitCellIndexer::removeBadUnitCells()
{
    const double quality = _parameters.indexing_threshold;

    // remove the bad solutions
    auto remove_bad_unit_cell =
        std::remove_if(_solutions.begin(), _solutions.end(), [=](const indexer_solution& s) {
            return s.second < quality;
        });
    _solutions.erase(remove_bad_unit_cell, _solutions.end());
}

void UserDefinedUnitCellIndexer::rankUnitCells()
{
    // Sort solutions by decreasing quality.
    // For equal quality, smallest volume is first
    std::sort(
        _solutions.begin(), _solutions.end(),
        [](const indexer_solution& s1, const indexer_solution& s2) -> bool {
            if (std::fabs(s1.second - s2.second) < 1.0e-6)
                return (s1.first->volume() < s2.first->volume());
            else
                return (s1.second > s2.second);
        });
}

void UserDefinedUnitCellIndexer::refineUnitCells()
{
    for (auto& p : _solutions) {

        auto unit_cell = p.first;

        PeakFilter peak_filter;
        PeakList filtered_peaks;
        filtered_peaks = peak_filter.enabled(_peaks, true);
        filtered_peaks =
            peak_filter.indexed(filtered_peaks, *unit_cell, unit_cell->indexingTolerance());

        Eigen::Matrix3d B = unit_cell->reciprocalBasis();

        std::vector<Eigen::RowVector3d> hkls;
        std::vector<Eigen::RowVector3d> q_vectors;

        for (auto peak : filtered_peaks) {
            auto q_vector = peak->q();
            MillerIndex hkld(q_vector, *unit_cell);
            hkls.emplace_back(hkld.rowVector().cast<double>());
            q_vectors.emplace_back(q_vector.rowVector());
        }

        // Lambda to compute residuals
        auto residuals = [&B, &hkls, &q_vectors](Eigen::VectorXd& f) -> int {
            int n = f.size() / 3;

            for (int i = 0; i < n; ++i) {
                auto dq = q_vectors[i] - hkls[i] * B;
                f(3 * i + 0) = dq(0);
                f(3 * i + 1) = dq(1);
                f(3 * i + 2) = dq(2);
            }
            return 0;
        };

        // Pass by address the parameters to be fitted to the parameter store
        FitParameters params;
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c)
                params.addParameter(&B(r, c));
        }

        // Sets the Minimizer with the parameters store and the size of the residual
        // vector
        Minimizer minimizer;
        minimizer.initialize(params, 3 * q_vectors.size());
        minimizer.set_f(residuals);
        minimizer.setxTol(1e-15);
        minimizer.setfTol(1e-15);
        minimizer.setgTol(1e-15);

        // fails to fit
        if (!minimizer.fit(500))
            continue;

        // Update the cell with the fitter one and reduce it
        try {
            unit_cell->setReciprocalBasis(B);
            unit_cell->setIndexingTolerance(_parameters.indexing_tolerance);
            unit_cell->reduce(
                _parameters.niggli_only, _parameters.niggli_tolerance,
                _parameters.gruber_tolerance);
            *unit_cell = unit_cell->applyNiggliConstraints();
            //            auto character = reduced_unit_cell.character();
            //            auto rec_basis = reduced_unit_cell.basis();
            //            std::cout<<"a* = "<<rec_basis.col(0).norm()<<std::endl;
            //            std::cout<<"b* = "<<rec_basis.col(1).norm()<<std::endl;
            //            std::cout<<"c* = "<<rec_basis.col(2).norm()<<std::endl;
            //
            //            std::cout<<character.a<<std::endl;
            //            std::cout<<character.b<<std::endl;
            //            std::cout<<character.c<<std::endl;
            //            std::cout<<character.alpha<<std::endl;
            //            std::cout<<character.beta<<std::endl;
            //            std::cout<<character.gamma<<std::endl;
            //            std::cout<<reduced_unit_cell.niggliCharacter().bravais<<std::endl;

        } catch (std::exception& e) {
            continue;
        }

        PeakList refiltered_peaks;
        refiltered_peaks =
            peak_filter.indexed(filtered_peaks, *unit_cell, unit_cell->indexingTolerance());

        double score = static_cast<double>(refiltered_peaks.size());
        double maxscore = static_cast<double>(filtered_peaks.size());

        // Percentage of indexing
        score /= 0.01 * maxscore;
        p.second = score;
    }
}

const std::vector<std::pair<sptrUnitCell, double>>& UserDefinedUnitCellIndexer::solutions() const
{
    return _solutions;
}

void UserDefinedUnitCellIndexer::run()
{
    if (_peaks.empty()) {
        nsx::error() << "No peaks vectors provided for indexing";
        return;
    }

    try {
        _parameters.checkParameters();
    } catch (const std::exception& e) {
        nsx::error() << e.what();
        return;
    }

    index();

    refineUnitCells();

    removeBadUnitCells();

    rankUnitCells();

    nsx::info() << _solutions.size() << "unit cells found";
}

} // namespace nsx
