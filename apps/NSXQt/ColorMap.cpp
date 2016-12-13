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


#include "ColorMap.h"
#include <ctime>
#include <cmath>
#include <array>

// helper function used for matplotlib-derived color maps
template<typename Arr>
static QRgb cmap_poly(const Arr& rs, const Arr& gs, const Arr& bs, double t)
{
    if (t < 0)
        t = 0.0;

    if ( t > 1.0)
        t = 1.0;

    assert(rs.size() == gs.size() && rs.size() == bs.size());
    assert(rs.size() >= 2);

    double r = 0.0, g = 0.0, b = 0.0;
    double tn = 1.0;

    for (int i = 0; i < rs.size(); ++i) {
        r += rs[i]*tn;
        g += gs[i]*tn;
        b += bs[i]*tn;
        tn *= t;
    }

    r *= 255.0;
    g *= 255.0;
    b *= 255.0;

    int ir = int(r+0.5);
    int ig = int(g+0.5);
    int ib = int(b+0.5);

    return qRgb(ir, ig, ib);
}

QImage ColorMap::matToImage(const Eigen::MatrixXi& source, const QRect& rect, int colorMax, bool log)
{
    // invalid rectangle: early return
    if (rect.left() < 0 || rect.top() < 0)
        return QImage();

    const int rows = source.rows();
    const int cols = source.cols();

    const int xmin = rect.left();
    const int xmax = rect.right();
    const int ymin = rect.top();
    const int ymax = rect.bottom();

    // invalid rectangle: early return
    if (xmax > cols || ymax > rows)
        return QImage();

    QImage dest(cols, rows, QImage::Format_RGB32);

    for (int y = ymin; y <= ymax; ++y) {
        QRgb* destrow = (QRgb*)(dest.scanLine(y-rect.top()));

        for (int x = xmin; x <= xmax; ++x) {
            double val = log ? std::log(source(y, x)) : source(y, x);
            destrow[x-xmin] = color(val, colorMax);
        }
    }

    return dest;
}

QRgb BlueWhiteCMap::color(double v, double vmax)
{
    if (v > vmax)
        return QColor(0, 0, 255).rgb();

    if (v < 0)
        v = 0.0;

    double mm = 1.0/vmax;
    long r =std::lround(255.0 - v*(255*mm));

    return qRgb(r, r, 255);
}

QRgb ViridisCMap::color(double v, double vmax)
{
    return cmap_poly(_r, _g, _b, v/vmax);
}


// what follows below are precomuted coefficients of polynomials giving greyscale -> RGB color maps
// taken by fitting cmap profiles from matplotlib. The color maps are named after the corresponding
// color maps in matplotlib

// viridis color map coefficients
const std::array<double, 10> ViridisCMap::_r = {
    0.26199569,   0.76259414,  -7.95549936,  28.42312215, -50.08511333,
      19.34939787,  43.08721907, -11.35989349, -50.97404283,  29.49258413
};

const std::array<double, 10> ViridisCMap::_g = {
    0.00418753,  1.4837755,  -0.8924195,  -0.1797478,  1.00381965,  0.59462808,
     -0.48846077, -1.07553209, -0.59690997,  1.05359406
};

const std::array<double, 10> ViridisCMap::_b = {
    0.32239281,   1.94232169,  -6.74189755 , 13.57191937, -14.96017142,
      1.05841996 , 13.84594501,  -2.33633239 ,-19.99350254,  13.43282862
};

