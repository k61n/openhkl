#pragma once

#include <array>
#include <map>
#include <utility>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include "Any.h"
#include "CrystalTypes.h"
#include "ReciprocalVector.h"
#include "UnitCell.h"

namespace nsx {

class UserDefinedUnitCellIndexer {

public:

    UserDefinedUnitCellIndexer();

    UserDefinedUnitCellIndexer(const std::map<std::string,Any>& parameters);

    UserDefinedUnitCellIndexer(const UserDefinedUnitCellIndexer& other) = default;

    UserDefinedUnitCellIndexer(UserDefinedUnitCellIndexer&& other) = default;

    ~UserDefinedUnitCellIndexer()=default;

    UserDefinedUnitCellIndexer& operator=(const UserDefinedUnitCellIndexer& other)=default;

    UserDefinedUnitCellIndexer& operator=(UserDefinedUnitCellIndexer&& other)=default;

    const std::map<std::string,Any>& parameters() const;
    void setParameters(const std::map<std::string,Any>& parameters);

    void run(const std::vector<ReciprocalVector>& q_vectors, double wavelength);

private:

    std::vector<UnitCell> index(const std::multimap<double,Eigen::RowVector3d> &q_vectors_mmap, double wavelength) const;

private:

    std::map<std::string,Any> _parameters;
};

} // end namespace nsx
