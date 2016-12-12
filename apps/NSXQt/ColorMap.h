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

inline QRgb BlueWhite(int v,int vmax)
{
    double mm=1.0/(double)vmax;
    int r=255-v*(255*mm);
    return (v>vmax ? 0xff0000ff : ((0xffu << 24) | r << 16 | r << 8 | 0xff ) );
}

inline QRgb ColorMap(double v, double vmax)
{
    v = std::max(v, 0.0);
    v = std::min(v, vmax);
    v /= vmax;

    const double b = 1.0 + 6 * (v*v*v/3.0 - v*v/2.0);
    const double r = 1.0 - b;
    const double g = 16.0 * v*v * (1-v)*(1-v);

    long lr = std::lround(255.0*r);
    long lg = std::lround(255.0*g);
    long lb = std::lround(255.0*b);

    return QColor(lr, lg, lb).rgb();
}

// Get an image from an 2D arrays of (rows,cols) with range (xmin:xmax,ymin:ymax) and
// intensity coded with colorMax as brightest.
QImage Mat2QImage(int* src, int rows, int cols, int xmin, int xmax, int ymin, int ymax, int colorMax, bool log=false);


#endif // COLORMAP_H
