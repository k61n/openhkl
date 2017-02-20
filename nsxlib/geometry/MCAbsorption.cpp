/*
 * MCAbsorption.cpp
 *
 *  Created on: Feb 18, 2015
 *      Author: chapon
 */

#include <iostream>
#include <ctime>

#include "../kernel/Error.h"
#include "MCAbsorption.h"

namespace SX
{
namespace Geometry
{
MCAbsorption::MCAbsorption(double width, double height, double pos):_sample(nullptr), _width(width),_height(height), _pos(pos), _muScat(0.0), _muAbs(0.0)
{
    _random=std::bind(std::uniform_real_distribution<double>(-0.5,0.5),std::mt19937(std::time(0)));
}

MCAbsorption::~MCAbsorption()
{
}

void MCAbsorption::setSample(ConvexHull<double>* sample, double muScat, double muAbs)
{
    _sample = sample;
    _muScat=muScat;
    _muAbs=muAbs;
}

double MCAbsorption::run(unsigned int nIterations, const Eigen::Vector3d& outV, const Eigen::Matrix3d& sampleOrientation) const
{

    TrianglesList faces=_sample->createFaceCache(sampleOrientation);

    if (faces.empty())
        throw SX::Kernel::Error<MCAbsorption>("No sample defined.");

    Eigen::Vector3d dir(0,1,0);

    double transmission(0.0);

    unsigned int nHits(0);

    for (unsigned int i=0;i<nIterations;++i)
    {
        double w=_random()*_width;
        double h=_random()*_height;
        Eigen::Vector3d point(w,_pos,h);

        unsigned int nIntersections(0);

        double times[2];

        for (const auto& triangle : faces)
        {
            if (triangle.isOutsideBB(w,h))
                continue;

            if (triangle.rayIntersect(point,dir,times[nIntersections]))
            {
                if (++nIntersections==2)
                    break;
            }
        }

        if (nIntersections!=2)
            continue;

        if (times[0]>times[1])
            std::swap(times[0],times[1]);

        double lpm=(0.5+_random())*(times[1]-times[0]);
        point+=lpm*dir + times[0]*dir;

        double t2;
        for (const auto& triangle : faces)
        {
            if (triangle.rayIntersect(point,outV,t2))
            {
                lpm+=outV.norm()*t2;
                nHits++;
                break;
            }
            else
                continue;
        }

        transmission+=exp(-(_muScat + _muAbs)*lpm);
    }

    if (nHits==0)
        transmission=1.0;
    else
        transmission /= nHits;

    return transmission;
}


} // Namespace Geometry

} // Namespace SX
