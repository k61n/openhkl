//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/instrument/Sample.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_INSTRUMENT_SAMPLE_H
#define CORE_INSTRUMENT_SAMPLE_H

#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "core/instrument/Component.h"
#include "core/hull/ConvexHull.h"
#include "core/data/DataTypes.h"

namespace nsx {

//! Helper struct for storing the result of the sample gonio fit
struct SampleGonioFit {
    bool success;
    std::vector<double> offsets;
    std::vector<double> cost_function;
};

//! \brief Class used to describe an experiment sample, consisting of a unit
//! cell and material.
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

    //! Returns a non-const reference to the convex hull describing the sample
    //! shape
    ConvexHull& shape();

    //! Returns a const reference to the convex hull describing the sample shape
    const ConvexHull& shape() const;

    SampleGonioFit fitGonioOffsets(
        const DataList& dataset, size_t n_iterations = 1000, double tolerance = 1.0e-6) const;

private:
    ConvexHull _sampleShape;
};

} // end namespace nsx

#endif // CORE_INSTRUMENT_SAMPLE_H
