//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      base/utils/Random.h
//! @brief     Generates random numbers
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//! @module-author tweber@ill.fr
//
//  ***********************************************************************************************

#ifndef OHKL_BASE_UTILS_RANDOM_H
#define OHKL_BASE_UTILS_RANDOM_H

#include <random>

namespace ohkl {

//! Random numbers

class Random {
 public:
    Random() = delete;
    ~Random() = delete;

    static std::mt19937& getRNG();

    static int intRange(int a = 0, int b = RAND_MAX);

    static double double01();
    static double doubleRange(double a, double b);

    static double gauss(double mean = 0., double sig = 1.);
    static int poisson(double mean = 0.);


 private:
    static std::random_device _randdev;
    static std::mt19937 _rng; // random number generator
};

} // namespace ohkl

#endif // OHKL_BASE_UTILS_RANDOM_H
