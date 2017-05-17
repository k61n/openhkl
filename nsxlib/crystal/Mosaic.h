/*
 * mosaic.h
 *
 *  Created on: Feb 10, 2016
 *      Author: pellegrini
 */

#ifndef MOSAIC_H_
#define MOSAIC_H_

#include <memory>
#include <string>
#include <vector>

#include "../geometry/Ellipsoid.h"
#include "../geometry/ConvexHull.h"

namespace nsx {

class DataSet;
class Diffractometer;
class Sample;

class Mosaic {

public:

    Mosaic(const std::string& instr, double l, double dl, double dMonSam, double mu);

    // bool run(const std::vector<std::string>& numors, unsigned int n, double& overlap);
    bool run(std::vector<std::shared_ptr<DataSet>> datas, unsigned int n, double& overlap);

    void setSample(Sample* sample);

    void setMosaicity(double mosaicity);

    virtual ~Mosaic();

private:

    std::shared_ptr<Diffractometer> _diffractometer;

    Sample* _sample;

    double _l;
    double _dl;
    double _dMonSam;
    double _mu;
};

double ellipsoids_overlap(const Ellipsoid<double,3>& ell1,const Ellipsoid<double,3>& ell2);

} // end namespace nsx

#endif /* MOSAIC_H_ */
