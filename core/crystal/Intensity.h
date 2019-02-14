#pragma once

namespace nsx {

//! \class Intensity
//! \brief Class to store the intensity of a given peak alongside with its
//! variance
class Intensity {
public:
  //! Constructs an Intensity from its value and variance.
  Intensity(double value = 0.0, double sigma2 = 0.0);

  //! Copy constructor.
  Intensity(const Intensity &other);

  //! Return the value of the Intensity
  double value() const;

  //! Return the standard deviation of the Intensity.
  double sigma() const;

  //! Return the variance of the Intensity
  double variance() const;

  //! Returns the sum of two Intensity whose value and variance are summed.
  Intensity operator+(const Intensity &other) const;
  //! Returns the difference between two Intensity whose value are subtracted
  //! and variance summed.
  Intensity operator-(const Intensity &other) const;
  //! Scale the Intensity by a factor. This will multiply the value by the
  //! factor and the variance by the square of the factor.
  Intensity operator*(double scale) const;
  //! Divide the Intensity by a factor. This will divide the value by the factor
  //! and the variance by the square of the factor.
  Intensity operator/(double denominator) const;

  //! Increment an Intensity with another Intensity by summing up their
  //! respective value and variance.
  Intensity &operator+=(const Intensity &other);
  //! Rescale the intensity
  Intensity &operator*=(double s);
  //! Divide the intensity, taking error into account
  Intensity operator/(const Intensity &other) const;

private:
  double _value;
  double _sigma2;
};

} // end namespace nsx
