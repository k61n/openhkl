#pragma once

#include <memory>
#include <vector>

namespace nsx {

class Component;
class ComponentState;
class Detector;
class Diffractometer;
class Experiment;
class Gonio;
struct InstrumentState;
class MonoDetector;
class RotAxis;
class TransAxis;
class Sample;
class Source;

using sptrDetector       = std::shared_ptr<Detector>;
using sptrGonio          = std::shared_ptr<Gonio>;
using sptrMonoDetector   = std::shared_ptr<MonoDetector>;
using sptrDiffractometer = std::shared_ptr<Diffractometer>;
using sptrExperiment     = std::shared_ptr<Experiment>;
using sptrSample         = std::shared_ptr<Sample>;
using sptrSource         = std::shared_ptr<Source>;

using uptrComponentState = std::unique_ptr<ComponentState>;
using InstrumentStateList = std::vector<InstrumentState>;

using DetectorState = std::vector<double>;

} // end namespace nsx
