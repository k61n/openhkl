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

#include "BrillouinZone.h"
#include "../crystal/NiggliReduction.h"

#include <Eigen/Eigenvalues>
#include <utility>
#include <iostream>


void remove_duplicates(std::vector<Eigen::RowVector3d>& vect, bool reflect, double eps)
{
    const double eps2 = eps*eps;
    const int n = vect.size();
    std::vector<Eigen::RowVector3d> new_vect;

    for (const auto v: vect) {
        bool duplicate = false;

        for (const auto w: new_vect) {
            if ((v-w).squaredNorm() < eps2 || (reflect && (v+w).squaredNorm() < eps2)) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            new_vect.emplace_back(v);
        }
    }
    std::swap(vect, new_vect);
}

namespace nsx {

BrillouinZone::BrillouinZone(const Eigen::Matrix3d& B, double eps): 
    _qs(),
    _eps(eps), 
    _B(B),
    _vertices()
{
    Eigen::Matrix3d A, G, P;
    A = B.inverse();
    G = A.transpose()*A;

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
    // outside of the bounding sphere
    if (q.squaredNorm() > (1+_eps)*_r2) {
        return false;
    }

    // check that it is in the convex hull
    for (auto&& q1: _qs) {
        if (std::fabs(q1.dot(q) / q1.dot(q1)) > 0.5+_eps) {
            return false;
        }
    }
    return true;
}

void BrillouinZone::clean_qs() 
{
    // remove duplicates, if any
    remove_duplicates(_qs, true, _eps);
    // remove external vertices
    std::remove_if(_qs.begin(), _qs.end(), [&](const Eigen::RowVector3d& v) { return !inside(0.5*v);});


    std::vector<Eigen::RowVector3d> new_qs;
    const auto n = _qs.size();

    // only include those vertices which are on the interior of a face
    for (auto i = 0; i < n; ++i) {
        Eigen::RowVector3d qi = 0.5*_qs[i];
        // check if it intersects more than one plane: superfluous if so
        unsigned int intersections = 0;
        for (auto j = 0; j < n; ++j) {
            Eigen::RowVector3d qj = _qs[j];
            // check whether the point qi lies on either of the Bragg planes given by +- qj
            if (std::fabs(std::fabs(qj.dot(qi) / qj.dot(qj))-0.5) < _eps) {
                ++intersections;
            }
        }
        if (intersections == 1) {
            new_qs.emplace_back(_qs[i]);
        }
    }

    std::swap(new_qs, _qs);
}

void BrillouinZone::compute()
{
    _qs.clear();    

    // start with initial parallelpiped; compute bounding sphere
    for (int i = 0; i < 3; ++i) {
        auto q = _B.row(i);
        _qs.emplace_back(q);  
    }

    // get vertices of parallelpiped
    compute_vertices();   
    _vertices.clear();
    assert(_r2 > 0.0);

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> solver(_B*_B.transpose());
    const double lambda = solver.eigenvalues().minCoeff();
    const double bound2 = 4.0 * _r2 / lambda + _eps;
    const double bound = std::sqrt(bound2);

    for (int h = 0; h <= bound; ++h) {
        for (int k = -bound-1; k <= bound; ++k) {
            for (int l = -bound-1; l <= bound; ++l) {

                auto hkl2 = h*h + k*k + l*l;

                if (hkl2 < 1 || hkl2 > bound2) {
                    continue;
                }

                Eigen::RowVector3d q = Eigen::RowVector3d(h,k,l) * _B;

                if (inside(0.5*q)) {
                    _qs.emplace_back(std::move(q));
                }
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
    for (auto&& q : _qs) {
        normals.emplace_back(-q);
    }

    const auto n = normals.size();

    Eigen::Matrix3d A;
    Eigen::Vector3d b;

    for (auto i = 0; i < n; ++i) {
        A.row(0) = normals[i];
        b(0) = -0.5*A.row(0).squaredNorm();
        for (auto j = i+1; j < n; ++j) {
            A.row(1) = normals[j];
            b(1) = -0.5*A.row(1).squaredNorm();
            for (auto k = j+1; k < n; ++k) {            
                A.row(2) = normals[k];
                b(2) = -0.5*A.row(2).squaredNorm();

                // check if the 3 planes intersect
                auto QR = A.colPivHouseholderQr();

                // rank != 3 => planes either do not intersect, or do not intersect at a unique point
                if (QR.rank() != 3) {
                    continue;
                }
                
                // get the point of intersection, check if it is inside the Brillouin Zone
                auto x = QR.solve(b);

                if (!inside((1.0-_eps)*x.transpose())) {
                    continue;                    
                }
                _vertices.emplace_back(x);
            }
        }
    }
    remove_duplicates(_vertices, false, _eps);

    // now compute radii
    _r2 = 0.0;

    for (auto v: _vertices) {
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
    for (auto v: _vertices) {
        hull.addVertex(v.transpose());
    }
    hull.updateHull();
    return hull;
}

double BrillouinZone::innerRadius() const
{
    double r2 = _qs[0].squaredNorm();

    for (auto q: _qs) {
        r2 = std::min(r2, q.squaredNorm());
    }
    return std::sqrt(r2/4.0);
}

double BrillouinZone::outerRadius() const
{
    assert(_vertices.size() != 0.0);
    assert(_r2 > 0.0);
    return std::sqrt(_r2);
}

}
