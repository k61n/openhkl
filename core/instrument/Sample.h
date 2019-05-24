#ifndef CORE_INSTRUMENT_SAMPLE_H
#define CORE_INSTRUMENT_SAMPLE_H

#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "Component.h"
#include "ConvexHull.h"
#include "DataTypes.h"

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

    //! Default constructor
    Sample();

    //! Copy constructor
    Sample(const Sample& other) = default;

    //! Constructs a default sample with a given name
    Sample(const std::string& name);

    //! Constructs a sample from a property tree node
    Sample(const YAML::Node& node);

    //! Virtual copy constructor
    Sample* clone() const;

    //! Destructor
    virtual ~Sample();

    //! Assignment operator
    Sample& operator=(const Sample& other) = default;

    //! Set the sample shape described as a convex hull
    void setShape(const ConvexHull& shape);

    //! Return a non-const reference to the convex hull describing the sample
    //! shape
    ConvexHull& shape();

    //! Return a const reference to the convex hull describing the sample shape
    const ConvexHull& shape() const;

    SampleGonioFit fitGonioOffsets(
        const DataList& dataset, size_t n_iterations = 1000, double tolerance = 1.0e-6) const;

private:
    ConvexHull _sampleShape;
};

} // end namespace nsx

#endif // CORE_INSTRUMENT_SAMPLE_H
