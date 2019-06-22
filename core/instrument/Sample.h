//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Sample.h
//! @brief     Defines class Sample
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INSTRUMENT_SAMPLE_H
#define CORE_INSTRUMENT_SAMPLE_H

#include "base/hull/ConvexHull.h"
#include "core/gonio/Component.h" // inheriting from

namespace nsx {

//! A sample, consisting of a `Gonio`meter (via Component) and a ConvexHull.

class Sample : public Component {
 public:
    //! Static constructor of a Sample from a property tree node
    static Sample* create(const YAML::Node& node);
    Sample();
    Sample(const Sample& other) = default;

    //! Constructs a default sample with a given name
    Sample(const std::string& name);

    //! Constructs a sample from a property tree node
    Sample(const YAML::Node& node);
    Sample* clone() const;
    virtual ~Sample();
    Sample& operator=(const Sample& other) = default;

    //! Sets the sample shape described as a convex hull
    void setShape(const ConvexHull& shape);

    //! Returns a non-const reference to the convex hull describing the sample shape
    ConvexHull& shape();

    //! Returns a const reference to the convex hull describing the sample shape
    const ConvexHull& shape() const;

 private:
    ConvexHull _sampleShape;
};

} // namespace nsx

#endif // CORE_INSTRUMENT_SAMPLE_H
