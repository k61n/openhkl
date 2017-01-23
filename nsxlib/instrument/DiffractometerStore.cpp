#include <initializer_list>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Diffractometer.h"
#include "DiffractometerStore.h"
#include "Error.h"
#include "Path.h"

namespace SX
{

namespace Instrument
{

namespace fs=boost::filesystem;
namespace pt=boost::property_tree;
namespace xml=boost::property_tree::xml_parser;

DiffractometerStore::DiffractometerStore()
: Kernel::Singleton<DiffractometerStore,Kernel::Constructor,Kernel::Destructor>()
{
}

sptrDiffractometer DiffractometerStore::buildDiffractomer(const std::string& name)
{
    fs::path diffractometersPath(Utils::Path::getDiffractometersPath());
    diffractometersPath/=name;
    diffractometersPath+=".xml";

    property_tree::ptree root;
    try {
        xml::read_xml(diffractometersPath.string(),root);
    }
    catch (const std::runtime_error& error)	{
        throw SX::Kernel::Error<DiffractometerStore>(error.what());
    }

    const property_tree::ptree& instrumentNode=root.get_child("instrument");
    sptrDiffractometer diffractometer(new Diffractometer(instrumentNode));
    return diffractometer;
}

diffractometersList DiffractometerStore::getDiffractometersList() const
{

    diffractometersList diffractometers;

    fs::path diffractometersPath(Utils::Path::getDiffractometersPath());

    for (const auto& p : boost::make_iterator_range(fs::directory_iterator(diffractometersPath),fs::directory_iterator()))
    {
        if (!fs::is_regular_file(p) || p.path().extension() != ".xml")
            continue;
        diffractometers.insert(p.path().stem().string());
    }

    return diffractometers;
}

} // end namespace Instrument

} // end namespace SX
