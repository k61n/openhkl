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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
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
  ColorMap(const std::string &name);
  ColorMap();
  ColorMap(const double *rgb);
  ~ColorMap();

  inline QRgb color(double v, double vmax) {
    int i = int(v / vmax * 255.0);
    i = std::max(0, i);
    i = std::min(255, i);
    return qRgb(_rgb[3 * i + 0], _rgb[3 * i + 1], _rgb[3 * i + 2]);
  }

  inline QRgb log_color(double v, double vmax) {
    double t = std::max(v / vmax * 255.0, 0.0);
    int i = std::min(255, int(t + 0.5));
    return qRgb(_log_rgb[3 * i + 0], _log_rgb[3 * i + 1], _log_rgb[3 * i + 2]);
  }

  QImage matToImage(const Eigen::ArrayXXd &source, const QRect &rect,
                    double colorMax, bool log = false);

  static ColorMap getColorMap(const std::string &name);
  static std::vector<std::string> getColorMapNames();

private:
  std::vector<double> _rgb;
  std::vector<double> _log_rgb;
};
