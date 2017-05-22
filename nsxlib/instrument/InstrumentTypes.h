#ifndef NSXLIB_INSTRUMENTTYPES_H
#define NSXLIB_INSTRUMENTTYPES_H

#include <memory>
#include <vector>

namespace nsx {

class Component;
class ComponentState;
class Detector;
class Diffractometer;
class Experiment;
class Gonio;
class InstrumentState;
class MonoDetector;
class RotAxis;
class TransAxis;
class Sample;
class Sample;
class Source;

using sptrDetector       = std::shared_ptr<Detector>;
using sptrGonio          = std::shared_ptr<Gonio>;
using sptrMonoDetector   = std::shared_ptr<MonoDetector>;
using sptrDiffractometer = std::shared_ptr<Diffractometer>;
using sptrExperiment     = std::shared_ptr<Experiment>;
using sptrSample         = std::shared_ptr<Sample>;
using sptrSource         = std::shared_ptr<Source>;

using InstrumentStateList = std::vector<InstrumentState>;

} // end namespace nsx

#endif // NSXLIB_INSTRUMENTTYPES_H
