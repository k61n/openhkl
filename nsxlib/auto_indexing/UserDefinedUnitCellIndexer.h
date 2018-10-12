#pragma once

#include <array>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include "CrystalTypes.h"
#include "ReciprocalVector.h"
#include "UnitCell.h"

namespace nsx {

class UserDefinedUnitCellIndexer {

public:

    UserDefinedUnitCellIndexer(double a, double b, double c, double alpha, double beta, double gamma, size_t n_solutions);

    UserDefinedUnitCellIndexer(double a, double b, double c, double alpha, double beta, double gamma, double distance_tolerance, double angular_tolerance, size_t n_solutions);

    UserDefinedUnitCellIndexer(const UserDefinedUnitCellIndexer& other) = default;

    UserDefinedUnitCellIndexer(UserDefinedUnitCellIndexer&& other) = default;

    ~UserDefinedUnitCellIndexer()=default;

    UserDefinedUnitCellIndexer& operator=(const UserDefinedUnitCellIndexer& other)=default;

    UserDefinedUnitCellIndexer& operator=(UserDefinedUnitCellIndexer&& other)=default;

    void setDistanceTolerance(double distance_tolerance);

    void setAngularTolerance(double angular_tolerance);

    void run(const std::vector<ReciprocalVector>& q_vectors, double wavelength);

private:

    std::vector<std::pair<Eigen::Matrix3d,Eigen::Matrix3d>> index(const std::vector<ReciprocalVector>& q_vectors, double wavelength) const;

    bool match_triplets(const Eigen::Matrix3d& b_triplet, const Eigen::Matrix3d& bu_triplet, Eigen::Matrix3d& b_matrix, Eigen::Matrix3d& bu_matrix) const;

private:

    UnitCell _unit_cell;

    double _distance_tolerance;

    double _angular_tolerance;

    size_t _n_solutions;
};

} // end namespace nsx
