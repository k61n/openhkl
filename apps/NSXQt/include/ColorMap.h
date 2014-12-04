#ifndef COLORMAP_H
#define COLORMAP_H
#include <QImage>


inline QRgb BlueMap(int v,int vmin,int vmax)
{
    if (v>vmax)
        return 0xff0000ff;
    if (v<vmin)
        return 0xffffffff;
    int r=(v-vmin)*255/(vmax-vmin);
    return ((0xff << 24) | ((255-r) & 0xff)<< 16 | ((255-r) & 0xff)<< 8 | 0xff );
}

// Get an image from an 2D arrays of (rows,cols) with range (xmin:xmax,ymin:ymax) and
// intensity coded with colorMax as brightest.
QImage Mat2QImage(int* src, int rows, int cols,int xmin, int xmax, int ymin, int ymax,int colorMax);


#endif // COLORMAP_H
