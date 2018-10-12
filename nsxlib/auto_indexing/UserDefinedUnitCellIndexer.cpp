#include <iostream>
#include <limits>
#include <stdexcept>

#include "Peak3D.h"
#include "UnitCell.h"
#include "UserDefinedUnitCellIndexer.h"

namespace nsx {

using indexer_solution = std::pair<Eigen::Matrix3d,Eigen::Matrix3d>;

UserDefinedUnitCellIndexer::UserDefinedUnitCellIndexer(double a, double b, double c, double alpha, double beta, double gamma, size_t n_solutions)
: _unit_cell(a,b,c,alpha,beta,gamma),
  _distance_tolerance(0.01),
  _angular_tolerance(0.01),
  _n_solutions(n_solutions)
{
}

UserDefinedUnitCellIndexer::UserDefinedUnitCellIndexer(double a,
                                                       double b,
                                                       double c,
                                                       double alpha,
                                                       double beta,
                                                       double gamma,
                                                       double distance_tolerance,
                                                       double angular_tolerance,
                                                       size_t n_solutions)
: _unit_cell(a,b,c,alpha,beta,gamma),
  _n_solutions(n_solutions)
{
    setDistanceTolerance(distance_tolerance);
    setAngularTolerance(angular_tolerance);
}

void UserDefinedUnitCellIndexer::setDistanceTolerance(double distance_tolerance) {

    if (distance_tolerance <= 0.0 || distance_tolerance >=1.0) {
        throw std::runtime_error("Invalid distance tolerance: must be in ]0,1[");
    }
    _distance_tolerance = distance_tolerance;

}

void UserDefinedUnitCellIndexer::setAngularTolerance(double angular_tolerance) {

    if (angular_tolerance <= 0.0 || angular_tolerance >=1.0) {
        throw std::runtime_error("Invalid angular tolerance: must be in ]0,1[");
    }
    _angular_tolerance = angular_tolerance;

}

bool UserDefinedUnitCellIndexer::match_triplets(const Eigen::Matrix3d& b_triplet, const Eigen::Matrix3d& bu_triplet, Eigen::Matrix3d& b_matrix, Eigen::Matrix3d& bu_matrix) const
{
    const double volume_tolerance = _distance_tolerance * _distance_tolerance * _distance_tolerance;

    const double det_b_triplet = b_triplet.determinant();
    const double det_bu_triplet = bu_triplet.determinant();

    // The two triplets does not match in volume, skip
    if (std::fabs((det_bu_triplet - det_b_triplet)/det_b_triplet) > volume_tolerance) {
        return false;
    }

    for (size_t i = 0; i < 3; ++i) {
        const double q1i_norm = b_triplet.row(i).norm();
        for (size_t j = 0; j < 3; ++j) {
            if (i==j) {
                continue;
            }
            const double q1j_norm = b_triplet.row(j).norm();

            for (size_t k = 0; k < 3; ++k) {
                if (i==k || j==k) {
                    continue;
                }
                const double q1k_norm = b_triplet.row(k).norm();

                for (size_t ii = 0; ii < 3; ++ii) {
                    const double q2i_norm = bu_triplet.row(ii).norm();

                    // Try to match triplet1 and triplet2 i vectors, if the match fails skip this experimental triplet
                    if (std::fabs((q2i_norm - q1i_norm)/q1i_norm) > _distance_tolerance) {
                        continue;
                    }

                    for (size_t jj = 0; jj < 3; ++jj) {
                        if (ii==jj) {
                            continue;
                        }
                        const double q2j_norm = bu_triplet.row(jj).norm();

                        // Try to match triplet1 and triplet2 j vectors, if the match fails skip this experimental triplet
                        if (std::fabs((q2j_norm - q1j_norm)/q1j_norm) > _distance_tolerance) {
                            continue;
                        }

                        for (size_t kk = 0; kk < 3; ++kk) {
                            if (ii==kk || jj==kk) {
                                continue;
                            }
                            const double q2k_norm = bu_triplet.row(kk).norm();

                            // Try to match triplet1 and triplet2 k vectors, if the match fails skip this experimental triplet
                            if (std::fabs((q2k_norm - q1k_norm)/q1k_norm) > _distance_tolerance) {
                                continue;
                            }

                            const double q1iq1j_angle = std::acos(b_triplet.row(i).dot(b_triplet.row(j))/q1i_norm/q1j_norm);
                            const double q2iq2j_angle = std::acos(bu_triplet.row(ii).dot(bu_triplet.row(jj))/q2i_norm/q2j_norm);

                            if (std::fabs((q2iq2j_angle - q1iq1j_angle)/q1iq1j_angle) > _angular_tolerance) {
                                continue;
                            }

                            const double q1iq1k_angle = std::acos(b_triplet.row(i).dot(b_triplet.row(k))/q1i_norm/q1k_norm);
                            const double q2iq2k_angle = std::acos(bu_triplet.row(ii).dot(bu_triplet.row(kk))/q2i_norm/q2k_norm);

                            if (std::fabs((q2iq2k_angle - q1iq1k_angle)/q1iq1k_angle) > _angular_tolerance) {
                                continue;
                            }

                            b_matrix.row(0) = b_triplet.row(i);
                            b_matrix.row(1) = b_triplet.row(j);
                            b_matrix.row(2) = b_triplet.row(k);

                            bu_matrix.row(0) = bu_triplet.row(ii);
                            bu_matrix.row(1) = bu_triplet.row(jj);
                            bu_matrix.row(2) = bu_triplet.row(kk);

                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

std::vector<indexer_solution> UserDefinedUnitCellIndexer::index(const std::vector<ReciprocalVector>& q_vectors, double wavelength) const
{
    // Compute the dmin and dmax form the input q vectors
    double dmin(std::numeric_limits<double>::infinity());
    double dmax(-std::numeric_limits<double>::infinity());
    for (auto&& q_vector : q_vectors) {
        const double d = 1.0/q_vector.rowVector().norm();
        dmin = std::min(d,dmin);
        dmax = std::max(d,dmax);
    }

    // Generate the q vectors from the initial B matrix within [dmin,dmax]
    // The generation uses the initial B matrix built from the uit cell parameters provided by the user hence without orientation matrix
    auto&& b_mat = _unit_cell.reciprocalBasis();
    std::vector<Eigen::RowVector3d> predicted_q_vectors;
    auto hkls = _unit_cell.generateReflectionsInShell(dmin,dmax,wavelength);
    for (auto hkl : hkls) {
        Eigen::RowVector3d q = hkl.rowVector().cast<double>() * b_mat;
        predicted_q_vectors.emplace_back(q);
    }

    std::vector<indexer_solution> solutions;
    solutions.reserve(_n_solutions);

    Eigen::Matrix3d b_triplet;

    Eigen::Matrix3d b_matrix;
    Eigen::Matrix3d bu_matrix;

    // Triple loop over the predicted q vectors to build predicted q triplet (aka qp)
    for (size_t i=0; i < predicted_q_vectors.size()-2; ++i) {

        b_triplet.row(0) = predicted_q_vectors.at(i);

        for (size_t j = i+1; j < predicted_q_vectors.size()-1; ++j) {

            b_triplet.row(1) = predicted_q_vectors.at(j);

            for (size_t k = j+1; k < predicted_q_vectors.size(); ++k) {

                b_triplet.row(2) = predicted_q_vectors.at(k);

                double det_b_triplet = b_triplet.determinant();
                if (std::fabs(det_b_triplet) < 1.0e-6) {
                    continue;
                }

                Eigen::Matrix3d bu_triplet;

                // Triple loop over the experimental q vectors to build experimental q triplets which will be match over the predicted q-triplet (aka qe)
                for (size_t ii = 0; ii < q_vectors.size()-2; ++ii) {

                    bu_triplet.row(0)  = q_vectors.at(ii).rowVector();

                    for (size_t jj = ii+1; jj < q_vectors.size()-1; ++jj) {

                        bu_triplet.row(1)  = q_vectors.at(jj).rowVector();

                        for (size_t kk = jj+1; kk < q_vectors.size(); ++kk) {

                            bu_triplet.row(2)  = q_vectors.at(kk).rowVector();

                            double det_bu_triplet = bu_triplet.determinant();
                            if (std::fabs(det_bu_triplet) < 1.0e-6) {
                                continue;
                            }

                            bool success = match_triplets(b_triplet,bu_triplet,b_matrix,bu_matrix);

                            if (!success) {
                                continue;
                            }

                            // The predicted and experimental triplets match, keep that solution
                            solutions.emplace_back(b_matrix,bu_matrix);
                            if (solutions.size() >= _n_solutions) {
                                std::cout<<solutions.size()<<" "<<_n_solutions<<std::endl;
                                return solutions;
                            }


                        }
                    }
                }
            }
        }
    }

    return solutions;
}

void UserDefinedUnitCellIndexer::run(const std::vector<ReciprocalVector>& q_vectors, double wavelength)
{
    if (q_vectors.empty()) {
        throw std::runtime_error("No q vectors provided for indexing");
    }

    if (wavelength <= 0.0) {
        throw std::runtime_error("Negative wavelength value");
    }

    auto solutions = index(q_vectors,wavelength);

    for (auto&& solution : solutions) {

        auto&& b_matrix = solution.first;
        auto&& bu_matrix = solution.second;


        auto QR = b_matrix.colPivHouseholderQr();

        Eigen::Matrix3d U = QR.solve(bu_matrix);

        std::cout<<U<<std::endl;
    }
}

} // end namespace nsx
