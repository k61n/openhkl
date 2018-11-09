#pragma once

#include <memory>
#include <vector>

namespace nsx {

class Component;
class Detector;
class DetectorEvent;
class Diffractometer;
class Experiment;
class Gonio;
class InstrumentState;
class MonoDetector;
class RotAxis;
class TransAxis;

using sptrDetector       = std::shared_ptr<Detector>;
using sptrMonoDetector   = std::shared_ptr<MonoDetector>;
using sptrDiffractometer = std::shared_ptr<Diffractometer>;
using sptrExperiment     = std::shared_ptr<Experiment>;

using InstrumentStateList = std::vector<InstrumentState>;

using DetectorState = std::vector<double>;

} // end namespace nsx
