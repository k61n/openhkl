#ifndef CORE_AXES_AXISFACTORY_H
#define CORE_AXES_AXISFACTORY_H

#include <string>

#include <yaml-cpp/yaml.h>

#include "Axis.h"
#include "Factory.h"
#include "Singleton.h"

namespace nsx {

//! Factory class used to create axes (rotational, translational)
class AxisFactory : public Factory<Axis, std::string, const YAML::Node&>,
                    public Singleton<AxisFactory, Constructor, Destructor> {
private:
    friend class Constructor<AxisFactory>;
    friend class Destructor<AxisFactory>;
    AxisFactory();
    ~AxisFactory();
};

} // end namespace nsx

#endif // CORE_AXES_AXISFACTORY_H
