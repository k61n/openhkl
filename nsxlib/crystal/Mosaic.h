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

namespace SX {
namespace Instrument {
class Diffractometer;
class Sample;
}

namespace Data {
class IData;
}

namespace Crystal {
class Mosaic {

public:

    Mosaic(const std::string& instr, double l, double dl, double dMonSam, double mu);

    // bool run(const std::vector<std::string>& numors, unsigned int n, double& overlap);
    bool run(std::vector<std::shared_ptr<SX::Data::IData>> datas, unsigned int n, double& overlap);

    void setSample(SX::Instrument::Sample* sample);

    void setMosaicity(double mosaicity);

    virtual ~Mosaic();

private:

    std::shared_ptr<SX::Instrument::Diffractometer> _diffractometer;

    SX::Instrument::Sample* _sample;

    double _l;
    double _dl;
    double _dMonSam;
    double _mu;
};

double ellipsoids_overlap(const SX::Geometry::Ellipsoid<double,3>& ell1,const SX::Geometry::Ellipsoid<double,3>& ell2);

} // namespace Crystal
} // namespace SX

#endif /* MOSAIC_H_ */
