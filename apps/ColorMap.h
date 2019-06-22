//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      apps/ColorMap.h
//! @brief     Defines class ColorMap
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

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
    ColorMap(const std::string& name);
    ColorMap();
    ColorMap(const double* rgb);
    ~ColorMap();

    inline QRgb color(double v, double vmax)
    {
        int i = int(v / vmax * 255.0);
        i = std::max(0, i);
        i = std::min(255, i);
        return qRgb(_rgb[3 * i + 0], _rgb[3 * i + 1], _rgb[3 * i + 2]);
    }

    inline QRgb log_color(double v, double vmax)
    {
        double t = std::max(v / vmax * 255.0, 0.0);
        int i = std::min(255, int(t + 0.5));
        return qRgb(_log_rgb[3 * i + 0], _log_rgb[3 * i + 1], _log_rgb[3 * i + 2]);
    }

    QImage
    matToImage(const Eigen::ArrayXXd& source, const QRect& rect, double colorMax, bool log = false);

    static ColorMap getColorMap(const std::string& name);
    static std::vector<std::string> getColorMapNames();

 private:
    std::vector<double> _rgb;
    std::vector<double> _log_rgb;
};
