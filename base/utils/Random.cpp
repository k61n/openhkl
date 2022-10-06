//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/Random.cpp
//! @brief     Generates random numbers
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//! @module-author tweber@ill.fr
//
//  ***********************************************************************************************

#include "base/utils/Random.h"

namespace ohkl {

std::random_device Random::_randdev;
std::mt19937 Random::_rng(Random::_randdev()); // random number generator


std::mt19937& Random::getRNG()
{
    return _rng;
}


/*
 * random int between a and b
 */
int Random::intRange(int a, int b)
{
    return std::uniform_int_distribution<int>(a, b)(Random::_rng);
}


/*
 * random double between 0 and 1
 */
double Random::double01()
{
    return doubleRange(0., 1.);
}


/*
 * random double between a and b
 */
double Random::doubleRange(double a, double b)
{
    return std::uniform_real_distribution<double>(a, b)(Random::_rng);
}


/*
 * gaussian distribution with given mean and sigma
 */
double Random::gauss(double mean, double sig)
{
    return std::normal_distribution<double>(mean, sig)(Random::_rng);
}


/*
 * poisson distribution with given mean
 */
int Random::poisson(double mean)
{
    return std::poisson_distribution<int>(mean)(Random::_rng);
}

} // namespace ohkl
