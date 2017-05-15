/*
 * nsxtool : Neutron Single Crystal analysis toolkit
 ------------------------------------------------------------------------------------------
 Copyright (C)
 2017- Laurent C. Chapon, Eric Pellegrini, Jonathan Fisher

 Institut Laue-Langevin
 BP 156
 6, rue Jules Horowitz
 38042 Grenoble Cedex 9
 France
 chapon[at]ill.fr
 pellegrini[at]ill.fr

 Forschungszentrum Juelich GmbH
 52425 Juelich
 Germany
 j.fisher[at]fz-juelich.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef NSXTOOL_INTENSITY_H_
#define NSXTOOL_INTENSITY_H_

namespace nsx {
namespace Crystal {

class Intensity {
public:
    Intensity(double value = 0.0, double sigma2 = 0.0);
    Intensity(const Intensity& other);

    double getValue() const;
    double getSigma() const;

    Intensity operator+(const Intensity& other) const;
    Intensity operator-(const Intensity& other) const;
    Intensity operator*(double scale) const;
    Intensity operator/(double denominator) const;

    Intensity& operator=(const Intensity& other);
    Intensity& operator+=(const Intensity& other);

private:
    double _value;
    double _sigma2;
};

} // namespace Crystal
} // namespace nsx

#endif // NSXTOOL_INTENSITY_H_
