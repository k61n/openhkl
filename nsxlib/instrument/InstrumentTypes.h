#ifndef NSXLIB_INSTRUMENTTYPES_H
#define NSXLIB_INSTRUMENTTYPES_H

#include <memory>

namespace nsx {

class Detector;
class Experiment;
class Sample;
class Source;

using sptrDetector   = std::shared_ptr<Detector>;
using sptrExperiment = std::shared_ptr<Experiment>;
using sptrSample     = std::shared_ptr<Sample>;
using sptrSource     = std::shared_ptr<Source>;

} // end namespace nsx

#endif // NSXLIB_INSTRUMENTTYPES_H
