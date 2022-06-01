//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      tables/crystal/BrillouinZone.cpp
//! @brief     Implements class BrillouinZone
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <iostream>
#include <utility>

#include <Eigen/Eigenvalues>

#include "base/geometry/ReciprocalVector.h"
#include "tables/crystal/BrillouinZone.h"
#include "tables/crystal/NiggliReduction.h"

namespace {
//! Update a list of Q-vectors by removing the duplicates.
//!
//! Two Q-Vectors are defined as duplicates if the norm of their difference is under
//! a given tolerance.
//! @param q_vectors the set of q_vectors from which duplicates will be removed
//! @param reflect   if true the opposite of each vectors are also checked for duplication
//! @param eps       the tolerance under which two vectors are defined as duplicate

void remove_duplicates(std::vector<Eigen::RowVector3d>& q_vectors, bool reflect, double eps)
{
    const double eps2 = eps * eps;
    std::vector<Eigen::RowVector3d> unique_q_vectors;

    for (const auto& v : q_vectors) {
        bool duplicate = false;

        for (const auto& w : unique_q_vectors) {
            if ((v - w).squaredNorm() < eps2 || (reflect && (v + w).squaredNorm() < eps2)) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate)
            unique_q_vectors.emplace_back(v);
    }
    std::swap(q_vectors, unique_q_vectors);
}

} // namespace

namespace nsx {

BrillouinZone::BrillouinZone(const Eigen::Matrix3d& B, double eps)
    : _qs(), _eps(eps), _B(B), _vertices()
{
    Eigen::Matrix3d A, G, P;
    A = B.inverse();
    G = A.transpose() * A;

    // to aid computation, we reduce to Niggli cell first
    NiggliReduction niggli(G, eps);
    niggli.reduce(G, P);
    _B = P.inverse() * _B;

    compute();
    clean_qs();
    compute_vertices();
}

bool BrillouinZone::inside(const Eigen::RowVector3d& q) const
{
    // first-pass check: the point is outside of the bounding sphere
    if (q.squaredNorm() > (1 + _eps) * _r2)
        return false;

    // second-pass check: the point is inside the convex hull
    for (const auto& q1 : _qs) {
        if (std::fabs(q1.dot(q) / q1.dot(q1)) > 0.5 + _eps)
            return false;
    }
    return true;
}

void BrillouinZone::clean_qs()
{
    // remove duplicates, if any
    remove_duplicates(_qs, true, _eps);
    // remove external vertices
    std::remove_if(_qs.begin(), _qs.end(), [&](const Eigen::RowVector3d& v) {
        return !inside(0.5 * v);
    });

    std::vector<Eigen::RowVector3d> new_qs;
    const auto n = _qs.size();

    // only include those vertices which are on the interior of a face
    for (size_t i = 0; i < n; ++i) {
        Eigen::RowVector3d qi = 0.5 * _qs[i];
        // check if it intersects more than one plane: superfluous if so
        unsigned int intersections = 0;
        for (size_t j = 0; j < n; ++j) {
            Eigen::RowVector3d qj = _qs[j];
            // Returns true if the point qi lies on either of the Bragg planes given by
            // +- qj
            if (std::fabs(std::fabs(qj.dot(qi) / qj.dot(qj)) - 0.5) < _eps) {
                ++intersections;
            }
        }
        if (intersections == 1)
            new_qs.emplace_back(_qs[i]);
    }

    std::swap(new_qs, _qs);
}

void BrillouinZone::compute()
{
    _qs.clear();

    // start with initial parallelepiped; compute bounding sphere
    for (int i = 0; i < 3; ++i) {
        auto q = _B.row(i);
        _qs.emplace_back(q);
    }

    // Gets vertices of parallelepiped
    compute_vertices();
    _vertices.clear();
    assert(_r2 > 0.0);

    // Compute the lowerbound for the norm of the B matrix which can defined as
    // the minimum stretch induced by B on any Q vector see for more explanation:
    //  -
    //  http://web.stanford.edu/class/archive/ee/ee263/ee263.1082/lectures/symm.pdf
    //  -
    //  https://math.stackexchange.com/questions/290267/need-help-understanding-matrix-norm-notation
    // By definition, sqrt(lmin)*||x|| < || [hkl]*B || < sqrt(lmax)*||x|| where
    // lmin and lmax are resp. the minimum and and maximum eigen vales of B.B^t
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(_B * _B.transpose());
    const double lambda = solver.eigenvalues().minCoeff();
    // we have sqrt(lmin)*||x|| < ||[hkl]*B|| = ||Q|| < Qmax ==> ||x|| <
    // Qmax/sqrt(lmin) Qmax/sqrt(lmin) is therefore the bound for searching for
    // new vertices of the BZ Qmax = 2*r ==> ||x|| << 4.0*Qmax^2/lmin
    const double bound2 = 4.0 * _r2 / lambda + _eps;
    const double bound = std::sqrt(bound2);

    // No need to store the lower side of Q space
    for (int h = 0; h <= bound; ++h) {
        for (int k = -bound - 1; k <= bound; ++k) {
            for (int l = -bound - 1; l <= bound; ++l) {
                // Compute the "squared-norm" of hkl integer vector
                auto hkl2 = h * h + k * k + l * l;

                // hkl < 1 => reject the hkl null-vector
                if (hkl2 < 1 || hkl2 > bound2)
                    continue;

                Eigen::RowVector3d q = Eigen::RowVector3d(h, k, l) * _B;

                if (inside(0.5 * q))
                    _qs.emplace_back(std::move(q));
            }
        }
    }
}

void BrillouinZone::compute_vertices()
{
    _r2 = 1e100;
    _vertices.clear();
    std::vector<Eigen::RowVector3d> normals = _qs;

    // add reflections, necessary for the code below
    for (const auto& q : _qs)
        normals.emplace_back(-q);

    const auto n = normals.size();

    Eigen::Matrix3d A;
    Eigen::Vector3d b;

    // Loop over each q-triplet and define the intersection of the corresponding
    // Bragg plane The problem is stated like so. Each q is the normal of a plane
    // (see https://en.wikipedia.org/wiki/File:Brillouin_zone.svg) and by
    // definition of BZ passes through q/2. Hence the equation of the plane is
    // q.(x-q/2) = 0 ==> q.x = 0.5*q^2
    for (size_t i = 0; i < n; ++i) {
        A.row(0) = normals[i];
        b(0) = 0.5 * A.row(0).squaredNorm();
        for (size_t j = i + 1; j < n; ++j) {
            A.row(1) = normals[j];
            b(1) = 0.5 * A.row(1).squaredNorm();
            for (size_t k = j + 1; k < n; ++k) {
                A.row(2) = normals[k];
                b(2) = 0.5 * A.row(2).squaredNorm();

                // Check if the 3 planes intersect
                // We have to solve the following system of equations:
                // q1.x = 0.5*q1^2
                // q2.x = 0.5*q2^2
                // q3.x = 0.5*q3^2
                // ==> A.X = B where A = [q1 q2 q3] (in row) and B = 0.5*[q1^2 q2^2
                // a3^2]
                auto QR = A.colPivHouseholderQr();

                // rank != 3 => planes either do not intersect, or do not intersect at a
                // unique point
                if (QR.rank() != 3)
                    continue;

                // Gets the point of intersection
                auto x = QR.solve(b);

                // check if it is inside the Brillouin Zone
                if (!inside((1.0 - _eps) * x.transpose()))
                    continue;
                _vertices.emplace_back(x);
            }
        }
    }
    remove_duplicates(_vertices, false, _eps);

    // now compute the radii
    _r2 = 0.0;

    for (const auto& v : _vertices) {
        const double v2 = v.squaredNorm();
        _r2 = std::max(_r2, v2);
    }
}

const std::vector<Eigen::RowVector3d>& BrillouinZone::vertices() const
{
    return _vertices;
}

const std::vector<Eigen::RowVector3d>& BrillouinZone::normals() const
{
    return _qs;
}

ConvexHull BrillouinZone::convexHull() const
{
    ConvexHull hull;
    for (auto v : _vertices)
        hull.addVertex(v.transpose());
    hull.updateHull();
    return hull;
}

double BrillouinZone::innerRadius() const
{
    double r2 = _qs[0].squaredNorm();
    for (const auto& q : _qs)
        r2 = std::min(r2, q.squaredNorm());
    return std::sqrt(r2 / 4.0);
}

double BrillouinZone::outerRadius() const
{
    assert(_vertices.size() != 0.0);
    assert(_r2 > 0.0);
    return std::sqrt(_r2);
}

} // namespace nsx
