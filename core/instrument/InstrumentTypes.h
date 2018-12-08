#pragma once

#include <memory>
#include <vector>

namespace nsx {

class Experiment;
using sptrExperiment = std::shared_ptr<Experiment>;

class InstrumentState;
using InstrumentStateList = std::vector<InstrumentState>;

using DetectorState = std::vector<double>;

} // end namespace nsx
