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

#pragma once

#include <cmath>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <QColor>
#include <QImage>
#include <QRect>

class ColorMap {
public:

    ColorMap();

    ColorMap(const std::string& name);

    ColorMap(const double* rgb);

    QRgb color(double v, double vmax);

    QRgb logColor(double v, double vmax);

    QImage matToImage(const Eigen::ArrayXXd& source, const QRect& rect, double colorMax, bool log=false);

    static ColorMap colorMap(const std::string& name);
    static std::vector<std::string> colorMaps();

private:
    std::vector<double> _rgb;
    std::vector<double> _log_rgb;
};
