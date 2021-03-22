//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Source.cpp
//! @brief     Implements class Source
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "core/instrument/Source.h"
#include "base/utils/Units.h"
#include "core/instrument/Monochromator.h"

namespace nsx {

Source::Source() : Component("source"), _monochromators(), _selectedMonochromator(0) { }

Source::Source(const std::string& name)
    : Component(name), _monochromators(), _selectedMonochromator(0)
{
}

Source::Source(const YAML::Node& node) : Component(node), _selectedMonochromator(0)
{
    // Loop over the "monochromator" nodes and add the corresponding pointer to
    // Monochromator objects to the Source
    for (const auto& subnode : node) {
        std::string subnodeName = subnode.first.as<std::string>();
        if (subnodeName.compare("monochromator") == 0) {
            Monochromator m(subnode.second);
            addMonochromator(m);
        }
    }
}

Source* Source::clone() const
{
    return new Source(*this);
}

const std::vector<Monochromator>& Source::monochromators() const
{
    return _monochromators;
}

int Source::nMonochromators() const
{
    return _monochromators.size();
}

void Source::setSelectedMonochromator(size_t i)
{
    if (i >= _monochromators.size())
        throw std::runtime_error("setSelectedMonochromator(): index i is out of range");
    _selectedMonochromator = i;
}

Monochromator& Source::selectedMonochromator()
{
    if (_selectedMonochromator >= _monochromators.size())
        throw std::runtime_error(
            "getSelectedMonochromator(): selected monochromator does not exist");
    return _monochromators[_selectedMonochromator];
}

const Monochromator& Source::selectedMonochromator() const
{
    if (_selectedMonochromator >= _monochromators.size())
        throw std::runtime_error(
            "getSelectedMonochromator(): selected monochromator does not exist");
    return _monochromators[_selectedMonochromator];
}

void Source::addMonochromator(Monochromator mono)
{
    _monochromators.push_back(mono);
}

} // namespace nsx
