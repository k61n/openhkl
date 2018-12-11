#include "DataSet.h"

namespace nsx {

//! Return a convolved frame
Eigen::MatrixXd convolvedFrame(
    DataSet& dataSet, std::size_t idx, const std::string& convolver_type,
    const std::map<std::string, double>& parameters);
}
