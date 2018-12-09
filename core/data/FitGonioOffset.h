#include "DataTypes.h"

namespace nsx {

//! Helper struct for storing the result of the sample gonio fit
struct DetectorGonioFit
{
    bool success;
    std::vector<double> offsets;
    std::vector<double> cost_function;
};

//! Helper struct for storing the result of the sample gonio fit
struct SampleGonioFit
{
    bool success;
    std::vector<double> offsets;
    std::vector<double> cost_function;
};

class Gonio;

DetectorGonioFit fitDetectorGonioOffsets(
    const Gonio& gonio, const DataList& dataset, size_t n_iterations, double tolerance);

SampleGonioFit fitSampleGonioOffsets(
    const Gonio& gonio, const DataList& dataset, size_t n_iterations, double tolerance);

} // namespace nsx
