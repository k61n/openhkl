//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
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

#include "core/instrument/Sample.h"
#include "core/raw/DataKeys.h"

namespace nsx {

Sample* Sample::create(const YAML::Node& node)
{
    return new Sample(node);
}

Sample::Sample() : Component(nsx::ym_sample), _sampleShape() { }

Sample::Sample(const std::string& name) : Component(name), _sampleShape() { }

Sample::Sample(const YAML::Node& node) : Component(node) { }

Sample* Sample::clone() const
{
    return new Sample(*this);
}

Sample::~Sample() = default;

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
