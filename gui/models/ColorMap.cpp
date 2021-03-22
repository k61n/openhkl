//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/models/ColorMap.cpp
//! @brief     Implements class ColorMap
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/models/ColorMap.h"

#include <QImage>
#include <cmath>

ColorMap::ColorMap(const double* rgb) : _rgb(256 * 3, 0), _log_rgb(256 * 3, 0)

{
    std::memcpy(&_rgb[0], rgb,
                256 * 3 * sizeof(double)); // warning: this might be dangerous
    const double ilog2 = 1.0 / std::log(2.0);

    for (unsigned int i = 0; i < 256; ++i) {
        const double t = (1.0 + i / 256.0);
        const double x = std::log(t) * ilog2 * 255.0;
        const unsigned int a = static_cast<unsigned int>(x);
        const unsigned int b = std::min(a + 1, 255U);
        const double eps = t - a;

        for (unsigned int j = 0; j < 3; ++j)
            _log_rgb[3 * i + j] = (1 - eps) * _rgb[3 * a + j] + eps * _rgb[3 * b + j];
    }
}

ColorMap::ColorMap(const std::string& name) : ColorMap(getColorMap(name)) { }

ColorMap::ColorMap() : ColorMap(getColorMapNames().front()) { }

ColorMap::~ColorMap() { }

QImage ColorMap::matToImage(
    const Eigen::ArrayXXd& source, const QRect& rect, double colorMax, bool log)
{
    // invalid rectangle: early return
    if (rect.left() < 0 || rect.top() < 0)
        return QImage();

    if (std::abs(colorMax) < 1.0e-9)
        return QImage();

    const int rows = int(source.rows());
    const int cols = int(source.cols());

    const int xmin = rect.left();
    const int xmax = rect.right();
    const int ymin = rect.top();
    const int ymax = rect.bottom();

    // invalid rectangle: early return
    if (xmax > cols || ymax > rows)
        return QImage();

    QImage dest(cols, rows, QImage::Format_RGB32);

    for (int y = ymin; y <= ymax; ++y) {
        QRgb* destrow = reinterpret_cast<QRgb*>(dest.scanLine(y - rect.top()));

        for (int x = xmin; x <= xmax; ++x) {
            if (log)
                destrow[x - xmin] = log_color(source(y, x), colorMax);
            else
                destrow[x - xmin] = color(source(y, x), colorMax);
        }
    }

    return dest;
}
