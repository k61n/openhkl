#pragma once

#include <map>
#include <memory>
#include <string>

#include "DataTypes.h"

namespace nsx {

class Threshold {

public:

    using sptrThreshold = std::shared_ptr<Threshold>;

    Threshold();

    virtual ~Threshold()=0;

    virtual double value(sptrDataSet dataset, int frame) const=0;

    //! Non-const getter for threshold parameters
    std::map<std::string,double>& parameters();
    void setParameters(const std::map<std::string,double>& parameters);

    virtual const char* name() const=0;

    //! Const getter for threshold parameters
    const std::map<std::string,double>& parameters() const;

protected:

    std::map<std::string,double> _parameters;
};

} // end namespace
