/*
 * nsxtool : Neutron Single Crystal analysis toolkit
    ------------------------------------------------------------------------------------------
    Copyright (C)
    2016- Laurent C. Chapon, Eric C. Pellegrini Institut Laue-Langevin
          Jonathan Fisher, Forschungszentrum Juelich GmbH
    BP 156
    6, rue Jules Horowitz
    38042 Grenoble Cedex 9
    France
    chapon[at]ill.fr
    pellegrini[at]ill.fr
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


#ifndef COLORMAP_H
#define COLORMAP_H

#include <QImage>
#include <QColor>
#include <cmath>

#include <Eigen/Core>
#include <array>


class ColorMap {
public:
    virtual ~ColorMap() {};

    virtual QRgb color(double v, double vmax) = 0;

    QImage matToImage(const Eigen::MatrixXi& source, const QRect& rect, int colorMax, bool log=false);
    QImage matToImage(const Eigen::MatrixXd& source, const QRect& rect, double colorMax, bool log=false);
};

class BlueWhiteCMap: public ColorMap {
public:
    virtual QRgb color(double v, double vmax) override;
};

class ViridisCMap: public ColorMap {
public:
    virtual QRgb color(double v, double vmax) override;
private:
    static const std::array<double, 10> _r, _g, _b;
};


class InfernoCMap: public ColorMap {
public:
    virtual QRgb color(double v, double vmax) override;
private:
    static const std::array<double, 10> _r, _g, _b;
};


#endif // COLORMAP_H
