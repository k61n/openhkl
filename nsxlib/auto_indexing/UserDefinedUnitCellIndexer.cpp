#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>

#include <Eigen/QR>

#include "Any.h"
#include "FitParameters.h"
#include "MillerIndex.h"
#include "Minimizer.h"
#include "Peak3D.h"
#include "PeakFilter.h"
#include "ReciprocalVector.h"
#include "UnitCell.h"
#include "UserDefinedUnitCellIndexer.h"
#include "Units.h"

namespace nsx {

UserDefinedUnitCellIndexer::UserDefinedUnitCellIndexer()
{
    _parameters.emplace("distance_tolerance",1.0e-2);
    _parameters.emplace("angular_tolerance" ,1.0e-2);

    _parameters.emplace("niggli_only",false);

    _parameters.emplace("niggli_tolerance",1.0e-3);
    _parameters.emplace("gruber_tolerance",4.0e-2);

    _parameters.emplace("n_solutions",size_t(10));
    _parameters.emplace("indexing_tolerance",2.0e-1);
    _parameters.emplace("indexing_threshold",9.0e-1);

    _parameters.emplace("a",10.0);
    _parameters.emplace("b",10.0);
    _parameters.emplace("c",10.0);

    _parameters.emplace("alpha",90.0*nsx::deg);
    _parameters.emplace("beta" ,90.0*nsx::deg);
    _parameters.emplace("gamma",90.0*nsx::deg);
}

UserDefinedUnitCellIndexer::UserDefinedUnitCellIndexer(const std::map<std::string,Any>& parameters)
: UserDefinedUnitCellIndexer()
{
    setParameters(parameters);
}

const std::map<std::string,Any>& UserDefinedUnitCellIndexer::parameters() const
{
    return _parameters;
}

void UserDefinedUnitCellIndexer::setParameters(const std::map<std::string,Any>& parameters)
{
    for (auto p : parameters) {
        auto it = _parameters.find(p.first);
        // This parameter is used
        if (it == _parameters.end()) {
            continue;
        }
        _parameters[p.first] = p.second;
    }
}

std::vector<UnitCell> UserDefinedUnitCellIndexer::index(const std::multimap<double,Eigen::RowVector3d>& q_vectors_mmap, double wavelength) const
{
    // Compute the dmin and dmax form the input q vectors
    double dmin(std::numeric_limits<double>::infinity());
    double dmax(-std::numeric_limits<double>::infinity());
    for (auto&& p : q_vectors_mmap) {
        const double d = 1.0/p.first;
        dmin = std::min(d,dmin);
        dmax = std::max(d,dmax);
    }

    dmin -= 1.0e-6;
    dmax += 1.0e-6;

    nsx::UnitCell unit_cell(_parameters.at("a").as<double>(),
                            _parameters.at("b").as<double>(),
                            _parameters.at("c").as<double>(),
                            _parameters.at("alpha").as<double>(),
                            _parameters.at("beta").as<double>(),
                            _parameters.at("gamma").as<double>());

    // Generate the q vectors from the initial B matrix within [dmin,dmax]
    // The generation uses the initial B matrix built from the uit cell parameters provided by the user hence without orientation matrix
    auto&& b_mat = unit_cell.reciprocalBasis();
    std::vector<std::pair<Eigen::RowVector3d,Eigen::RowVector3d>> predicted_q_vectors;
    auto hkls = unit_cell.generateReflectionsInShell(dmin,dmax,wavelength);
    for (auto hkl : hkls) {
        Eigen::RowVector3d q = hkl.rowVector().cast<double>() * b_mat;
        predicted_q_vectors.emplace_back(hkl.rowVector().cast<double>(),q);
    }

    std::sort(predicted_q_vectors.begin(),predicted_q_vectors.end(),[](const std::pair<Eigen::RowVector3d,Eigen::RowVector3d>& v1,
                                                                       const std::pair<Eigen::RowVector3d,Eigen::RowVector3d>& v2){return v1.second.norm() < v2.second.norm();});

    std::vector<UnitCell> solutions;
    solutions.reserve(_parameters.at("n_solutions").as<size_t>());

    Eigen::Matrix3d b_triplet;

    Eigen::Matrix3d matching_hkls;
    Eigen::Matrix3d matching_q_vectors;

    const double distance_tolerance = _parameters.at("distance_tolerance").as<double>();
    const double angular_tolerance = _parameters.at("angular_tolerance").as<double>();

    const double volume_tolerance = distance_tolerance * distance_tolerance * distance_tolerance;

    // Triple loop over the predicted q vectors to build predicted q triplet (aka qp)
    for (size_t i = 0; i < predicted_q_vectors.size()-2; ++i) {

        b_triplet.row(0) = predicted_q_vectors[i].second;

        const double qi_norm = b_triplet.row(0).norm();
        auto lit_i = q_vectors_mmap.lower_bound(qi_norm*(1.0-distance_tolerance));
        auto uit_i = q_vectors_mmap.upper_bound(qi_norm*(1.0+distance_tolerance));

        for (size_t j = i+1; j < predicted_q_vectors.size()-1; ++j) {

            b_triplet.row(1) = predicted_q_vectors[j].second;

            const double qj_norm = b_triplet.row(1).norm();
            auto lit_j = q_vectors_mmap.lower_bound(qj_norm*(1.0-distance_tolerance));
            auto uit_j = q_vectors_mmap.upper_bound(qj_norm*(1.0+distance_tolerance));

            for (size_t k = j+1; k < predicted_q_vectors.size(); ++k) {

                b_triplet.row(2) = predicted_q_vectors[k].second;

                const double qk_norm = b_triplet.row(2).norm();
                auto lit_k = q_vectors_mmap.lower_bound(qk_norm*(1.0-distance_tolerance));
                auto uit_k = q_vectors_mmap.upper_bound(qk_norm*(1.0+distance_tolerance));

                double det_b_triplet = b_triplet.determinant();
                if (det_b_triplet < 1.0e-6) {
                    continue;
                }

                std::array<size_t,3> indexes({i,j,k});

                Eigen::Matrix3d bu_triplet;

                // Triple loop over the experimental q vectors to build experimental q triplets which will be match over the predicted q-triplet (aka qe)
                for (auto it_i = lit_i; it_i != uit_i; ++it_i) {

                    bu_triplet.row(0)  = it_i->second;;

                    for (auto it_j = lit_j; it_j != uit_j; ++it_j) {

                        bu_triplet.row(1)  = it_j->second;

                        for (auto it_k = lit_k; it_k != uit_k; ++it_k) {

                            bu_triplet.row(2)  = it_k->second;

                            double det_bu_triplet = bu_triplet.determinant();
                            if (det_bu_triplet < 1.0e-6) {
                                continue;
                            }

                            // The two triplets does not match in volume, skip
                            if (std::fabs((det_bu_triplet - det_b_triplet)/det_b_triplet) > volume_tolerance) {
                                continue;
                            }

                            for (size_t ti = 0; ti < 3; ++ti) {

                                const double q1i_norm = b_triplet.row(ti).norm();

                                for (size_t tj = 0; tj < 3; ++tj) {

                                    if (ti==tj) {
                                        continue;
                                    }

                                    const double q1j_norm = b_triplet.row(tj).norm();

                                    for (size_t tk = 0; tk < 3; ++tk) {

                                        if (ti==tk || tj==tk) {
                                            continue;
                                        }

                                        const double q1k_norm = b_triplet.row(tk).norm();

                                        for (size_t tii = 0; tii < 3; ++tii) {

                                            const double q2i_norm = bu_triplet.row(tii).norm();

                                            // Try to match triplet1 and triplet2 i vectors, if the match fails skip this experimental triplet
                                            if (std::fabs((q2i_norm - q1i_norm)/q1i_norm) > distance_tolerance) {
                                                continue;
                                            }

                                            for (size_t tjj = 0; tjj < 3; ++tjj) {

                                                if (tii==tjj) {
                                                    continue;
                                                }

                                                const double q2j_norm = bu_triplet.row(tjj).norm();

                                                // Try to match triplet1 and triplet2 j vectors, if the match fails skip this experimental triplet
                                                if (std::fabs((q2j_norm - q1j_norm)/q1j_norm) > distance_tolerance) {
                                                    continue;
                                                }

                                                for (size_t tkk = 0; tkk < 3; ++tkk) {

                                                    if (tii==tkk || tjj==tkk) {
                                                        continue;
                                                    }

                                                    const double q2k_norm = bu_triplet.row(tkk).norm();

                                                    // Try to match triplet1 and triplet2 k vectors, if the match fails skip this experimental triplet
                                                    if (std::fabs((q2k_norm - q1k_norm)/q1k_norm) > distance_tolerance) {
                                                        continue;
                                                    }

                                                    const double q1iq1j_angle = std::acos(b_triplet.row(ti).dot(b_triplet.row(tj))/q1i_norm/q1j_norm);
                                                    const double q2iq2j_angle = std::acos(bu_triplet.row(tii).dot(bu_triplet.row(tjj))/q2i_norm/q2j_norm);
                                                    if (std::fabs((q2iq2j_angle - q1iq1j_angle)/q1iq1j_angle) > angular_tolerance) {
                                                        continue;
                                                    }

                                                    const double q1iq1k_angle = std::acos(b_triplet.row(ti).dot(b_triplet.row(tk))/q1i_norm/q1k_norm);
                                                    const double q2iq2k_angle = std::acos(bu_triplet.row(tii).dot(bu_triplet.row(tkk))/q2i_norm/q2k_norm);
                                                    if (std::fabs((q2iq2k_angle - q1iq1k_angle)/q1iq1k_angle) > angular_tolerance) {
                                                        continue;
                                                    }

                                                    const double q1jq1k_angle = std::acos(b_triplet.row(tj).dot(b_triplet.row(tk))/q1j_norm/q1k_norm);
                                                    const double q2jq2k_angle = std::acos(bu_triplet.row(tjj).dot(bu_triplet.row(tkk))/q2j_norm/q2k_norm);
                                                    if (std::fabs((q2jq2k_angle - q1jq1k_angle)/q1jq1k_angle) > angular_tolerance) {
                                                        continue;
                                                    }

                                                    size_t m_i = indexes[ti];
                                                    size_t m_j = indexes[tj];
                                                    size_t m_k = indexes[tk];

                                                    matching_hkls.row(0) = predicted_q_vectors[m_i].first;
                                                    matching_hkls.row(1) = predicted_q_vectors[m_j].first;
                                                    matching_hkls.row(2) = predicted_q_vectors[m_k].first;

                                                    matching_q_vectors.row(0) = bu_triplet.row(tii);
                                                    matching_q_vectors.row(1) = bu_triplet.row(tjj);
                                                    matching_q_vectors.row(2) = bu_triplet.row(tkk);

                                                    auto BtUt = matching_hkls.inverse() * matching_q_vectors;

                                                    if (BtUt.determinant() < 0) {
                                                        continue;
                                                    }

                                                    nsx::UnitCell uc(BtUt,true);

                                                    double n_indexed_q_vectors(0);
                                                    for (auto q : q_vectors_mmap) {
                                                        MillerIndex miller_index(nsx::ReciprocalVector(q.second), uc);
                                                        if (miller_index.indexed(_parameters.at("indexing_tolerance").as<double>())) {
                                                            ++n_indexed_q_vectors;
                                                        }
                                                    }

                                                    const double quality = n_indexed_q_vectors/static_cast<double>(q_vectors_mmap.size());
                                                    if (quality < _parameters.at("indexing_threshold").as<double>()) {
                                                        continue;
                                                    }

                                                    // The predicted and experimental triplets match, keep that solution
                                                    solutions.push_back(uc);
                                                    if (solutions.size() >= _parameters.at("n_solutions").as<size_t>()) {
                                                        return solutions;
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

    std::multimap<double,Eigen::RowVector3d> q_vectors_mmap;
    for (auto q_vector : q_vectors) {
        Eigen::RowVector3d row_vect = q_vector.rowVector();
        q_vectors_mmap.emplace(row_vect.norm(),row_vect);
    }

    auto solutions = index(q_vectors_mmap,wavelength);

    for (auto&& uc : solutions) {

        Eigen::Matrix3d B = uc.reciprocalBasis();

        std::vector<Eigen::RowVector3d> hkls;
        std::vector<Eigen::RowVector3d> qs;
        std::vector<Eigen::Matrix3d> wt;

        for (auto q_vector : q_vectors) {
            MillerIndex hkld(q_vector, uc);
            hkls.emplace_back(hkld.rowVector().cast<double>());
            qs.emplace_back(q_vector.rowVector());
        }

        // Lambda to compute residuals
        auto residuals = [&B, &hkls, &qs] (Eigen::VectorXd& f) -> int
        {
            int n = f.size() / 3;

            for (int i = 0; i < n; ++i) {
                auto dq = qs[i] - hkls[i]*B;
                f(3*i+0) = dq(0);
                f(3*i+1) = dq(1);
                f(3*i+2) = dq(2);
            }
            return 0;
        };

        // Pass by address the parameters to be fitted to the parameter store
        FitParameters params;
        for (int r = 0; r < 3; ++r) {
            for (int c = 0; c < 3; ++c) {
                params.addParameter(&B(r,c));
            }
        }

        // Set the Minimizer with the parameters store and the size of the residual vector
        Minimizer minimizer;
        minimizer.initialize(params, 3*q_vectors.size());
        minimizer.set_f(residuals);
        minimizer.setxTol(1e-15);
        minimizer.setfTol(1e-15);
        minimizer.setgTol(1e-15);

        // fails to fit
        if (!minimizer.fit(500)) {
            continue;
        }

        // Update the cell with the fitter one and reduce it
        try {
            uc.setReciprocalBasis(B);
            uc.setIndexingTolerance(_parameters["indexing_tolerance"].as<double>());
            uc.reduce(_parameters["niggli_only"].as<bool>(), _parameters["niggli_tolerance"].as<double>(),_parameters["gruber_tolerance"].as<double>());
            auto reduced_unit_cell = uc.applyNiggliConstraints();
            auto character = reduced_unit_cell.character();
            auto rec_basis = reduced_unit_cell.basis();
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

        } catch(std::exception& e) {
            continue;
        }
    }
}

} // end namespace nsx
