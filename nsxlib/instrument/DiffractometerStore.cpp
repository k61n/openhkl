#include <initializer_list>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>

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

    boost::filesystem::path diffractometersPath(Path::getDiffractometersPath());
    diffractometersPath/=name;
    diffractometersPath+=".yaml";

    YAML::Node instrumentDefinition;

    try {
        instrumentDefinition = YAML::LoadFile(diffractometersPath.string());
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

std::set<std::string> DiffractometerStore::getDiffractometersList() const
{
    using boost::filesystem::directory_iterator;

    std::set<std::string> diffractometers;

    boost::filesystem::path diffractometersPath(Path::getDiffractometersPath());

    for (const auto& p : boost::make_iterator_range(directory_iterator(diffractometersPath),directory_iterator()))
    {
        if (!boost::filesystem::is_regular_file(p) || p.path().extension() != ".xml")
            continue;
        diffractometers.insert(p.path().stem().string());
    }

    return diffractometers;
}

} // end namespace nsx

