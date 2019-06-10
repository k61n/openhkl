//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      core/monte-carlo/mosaic.h
//! @brief     Defines ###CLASSES###
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef CORE_MONTE_CARLO_MOSAIC_H
#define CORE_MONTE_CARLO_MOSAIC_H

#include "core/instrument/Sample.h"

namespace nsx {

class Diffractometer;

//! Class to predict peak profiles (intensity distribution) using
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

#endif // CORE_MONTE_CARLO_MOSAIC_H
