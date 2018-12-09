#include "DataTypes.h"

namespace nsx {

//! Helper struct for storing the result of the sample gonio fit
struct GonioFitOutcome
{
    bool success;
    std::vector<double> offsets;
    std::vector<double> cost_function;
};

class Gonio;

GonioFitOutcome fitDetectorGonioOffsets(
    const Gonio& gonio, const DataList& dataset, size_t n_iterations, double tolerance);

GonioFitOutcome fitSampleGonioOffsets(
    const Gonio& gonio, const DataList& dataset, size_t n_iterations, double tolerance);

} // namespace nsx
