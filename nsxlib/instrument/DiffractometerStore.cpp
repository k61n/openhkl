#include <initializer_list>
#include <stdexcept>

#include "yaml-cpp/yaml.h"

#include "../instrument/Diffractometer.h"
#include "../instrument/DiffractometerStore.h"
#include "../utils/Path.h"

namespace nsx {

DiffractometerStore::DiffractometerStore() : Singleton<DiffractometerStore,Constructor,Destructor>()
{
}

sptrDiffractometer DiffractometerStore::buildDiffractometer(const std::string& name) const
{

    std::string diffractometerFile = buildPath(applicationDataPath(),{"instruments",name+".yaml"});

    YAML::Node instrumentDefinition;

    try {
        instrumentDefinition = YAML::LoadFile(diffractometerFile);
    }
    catch (const std::exception& error)	{
        throw std::runtime_error("Error when opening instrument definition file");
    }

    if (!instrumentDefinition["instrument"]) {
        throw std::runtime_error("Invalid instrument definition: missing 'instrument root node'");
    }

    sptrDiffractometer diffractometer;

    try {
        diffractometer = std::make_shared<Diffractometer>(Diffractometer(instrumentDefinition["instrument"]));
    }
    catch (...)
    {
        throw std::runtime_error("Error when reading instrument definition file");
    }

    return diffractometer;
}

} // end namespace nsx

