/*
 * mosaic.h
 *
 *  Created on: Feb 10, 2016
 *      Author: pellegrini
 */

#ifndef NSXLIB_MOSAIC_H
#define NSXLIB_MOSAIC_H

#include <string>
#include <vector>

#include "../data/DataTypes.h"
#include "../geometry/GeometryTypes.h"
#include "../geometry/ConvexHull.h"
#include "../instrument/InstrumentTypes.h"

namespace nsx {

class Mosaic {

public:

    Mosaic(const std::string& instr, double l, double dl, double dMonSam, double mu);

    bool run(DataList datas, unsigned int n, double& overlap);

    void setSample(Sample* sample);

    void setMosaicity(double mosaicity);

    virtual ~Mosaic()=default;

private:

    sptrDiffractometer _diffractometer;

    Sample* _sample;

    double _l;
    double _dl;
    double _dMonSam;
    double _mu;
};

double ellipsoids_overlap(const Ellipsoid& ell1,const Ellipsoid& ell2);

} // end namespace nsx

#endif // NSXLIB_MOSAIC_H
