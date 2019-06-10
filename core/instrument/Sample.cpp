//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Sample.cpp
//! @brief     Implements class Sample
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <stdexcept>

#include "core/experiment/DataSet.h"
#include "core/fitting/FitParameters.h"
#include "core/fitting/Minimizer.h"
#include "core/gonio/Gonio.h"
#include "core/instrument/Sample.h"
#include "core/logger/Logger.h"

namespace nsx {

Sample* Sample::create(const YAML::Node& node)
{
    return new Sample(node);
}

Sample::Sample() : Component("sample"), _sampleShape() {}

Sample::Sample(const std::string& name) : Component(name), _sampleShape() {}

Sample::Sample(const YAML::Node& node) : Component(node) {}

Sample* Sample::clone() const
{
    return new Sample(*this);
}

Sample::~Sample() {}

void Sample::setShape(const ConvexHull& shape)
{
    _sampleShape = shape;
}

ConvexHull& Sample::shape()
{
    return _sampleShape;
}

const ConvexHull& Sample::shape() const
{
    return _sampleShape;
}

} // namespace nsx
