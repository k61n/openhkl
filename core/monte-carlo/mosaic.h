#pragma once

#include <vector>

#include "Sample.h"

namespace nsx {

class Diffractometer;

//! \brief Class to predict peak profiles (intensity distribution) using
//! Monte-Carlo integration.
class Mosaic {

public:
    Mosaic(const std::string& instr, double l, double dl, double dMonSam, double mu);
    //! Run the Monte-Carlo simulation
    bool run(const std::vector<std::string>& numors, unsigned int n, double& overlap);

    void setSample(Sample* sample);

    void setMosaicity(double mosaicity);

    virtual ~Mosaic();

private:
    Diffractometer* _diffractometer;

    Sample* _sample;

    double _l;
    double _dl;
    double _dMonSam;
    double _mu;
};

double ellipsoids_overlap(const Ellipsoid& ell1, const Ellipsoid& ell2);

} // namespace nsx
