//  ***********************************************************************************************
//
//  OpenHKL: data reduction for single crystal diffraction
//
//! @file      core/peak/Intensity.h
//! @brief     Defines class Intensity
//!
//! @homepage  https://openhkl.org
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#ifndef OHKL_CORE_PEAK_INTENSITY_H
#define OHKL_CORE_PEAK_INTENSITY_H

#include <stdexcept>

namespace ohkl {

//! Store intensity and variance of a given peak.

class Intensity {
 public:
    //! Construct a default *invalid* intensity
    Intensity();

    //! Constructs an Intensity from its value and variance.
    Intensity(const double value, const double sigma2 = 0.0);

    //! Returns the value of the Intensity
    double value() const;

    //! Returns the standard deviation of the Intensity.
    double sigma() const;

    //! Returns the variance of the Intensity
    double variance() const;

    //! Return the strength (I/sigma)
    double strength() const;

    //! Returns the sum of two Intensity whose value and variance are summed.
    Intensity operator+(const Intensity& other) const;
    //! Returns the difference between two Intensity whose value are subtracted
    //! and variance summed.
    Intensity operator-(const Intensity& other) const;
    //! Scale the Intensity by a factor. This will multiply the value by the
    //! factor and the variance by the square of the factor.
    Intensity operator*(double scale) const;
    //! Divide the Intensity by a factor. This will divide the value by the factor
    //! and the variance by the square of the factor.
    Intensity operator/(double denominator) const;

    //! Increment an Intensity with another Intensity by summing up their
    //! respective value and variance.
    Intensity& operator+=(const Intensity& other);
    //! Rescale the intensity
    Intensity& operator*=(double s);
    //! Divide the intensity, taking error into account
    Intensity operator/(const Intensity& other) const;
    //! Whether the corrected intensity if valid (check for interpolation error)
    bool isValid() const;

 private:
    static constexpr double _eps = 1.0e-8;
    double _value;
    double _sigma2;
    bool _valid;
};

} // namespace ohkl

#endif // OHKL_CORE_PEAK_INTENSITY_H
