#include <initializer_list>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "../instrument/Diffractometer.h"
#include "../instrument/DiffractometerStore.h"
#include "../utils/Path.h"

namespace nsx {

using boost::filesystem::path;
using boost::filesystem::directory_iterator;
using boost::filesystem::is_regular_file;
using boost::property_tree::xml_parser::read_xml;

DiffractometerStore::DiffractometerStore() : Singleton<DiffractometerStore,Constructor,Destructor>()
{
}

sptrDiffractometer DiffractometerStore::buildDiffractometer(const std::string& name) const
{

    path diffractometersPath(Path::getDiffractometersPath());
    diffractometersPath/=name;
    diffractometersPath+=".xml";

    boost::property_tree::ptree root;
    try {
        read_xml(diffractometersPath.string(),root);
    }
    catch (const std::runtime_error& error)	{
        throw std::runtime_error(error.what());
    }

    const auto& instrumentNode=root.get_child("instrument");
    sptrDiffractometer diffractometer(new Diffractometer(instrumentNode));
    return diffractometer;
}

std::set<std::string> DiffractometerStore::getDiffractometersList() const
{

    std::set<std::string> diffractometers;

    path diffractometersPath(Path::getDiffractometersPath());

    for (const auto& p : boost::make_iterator_range(directory_iterator(diffractometersPath),directory_iterator()))
    {
        if (!is_regular_file(p) || p.path().extension() != ".xml")
            continue;
        diffractometers.insert(p.path().stem().string());
    }

    return diffractometers;
}

} // end namespace nsx

