#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "Detector.h"
#include "Factory.h"
#include "Singleton.h"

namespace nsx {

//! \brief Factory class for producing different detector geometries.
class DetectorFactory : public Factory<Detector, std::string, const YAML::Node&>,
                        public Singleton<DetectorFactory, Constructor, Destructor> {
private:
    friend class Constructor<DetectorFactory>;
    friend class Destructor<DetectorFactory>;
    DetectorFactory();
    ~DetectorFactory();
};

} // end namespace nsx
