//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/instrument/Sample.h
//! @brief     Defines class Sample
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_INSTRUMENT_SAMPLE_H
#define OHKL_CORE_INSTRUMENT_SAMPLE_H

#include "base/geometry/ConvexHull.h"
#include "core/gonio/Component.h" // inheriting from

namespace ohkl {

//! A sample, consisting of a `Gonio`meter (via Component) and a ConvexHull.

class Sample : public Component {
 public:
    //! Static constructor of a Sample from a property tree node
    Sample() = delete;
    static Sample* create(const YAML::Node& node);

    //! Constructs a sample from a property tree node
    Sample(const YAML::Node& node);
    Sample* clone() const;
    ~Sample();

    //! Sets the sample shape described as a convex hull
    void setShape(const ConvexHull& shape);

    //! Returns a non-const reference to the convex hull describing the sample shape
    ConvexHull& shape();

    //! Returns a const reference to the convex hull describing the sample shape
    const ConvexHull& shape() const;

 private:
    ConvexHull _sampleShape;
};

} // namespace ohkl

#endif // OHKL_CORE_INSTRUMENT_SAMPLE_H
