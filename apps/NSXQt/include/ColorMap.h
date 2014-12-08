#ifndef COLORMAP_H
#define COLORMAP_H
#include <QImage>


inline QRgb BlueWhite(int v,int vmax)
{
    double mm=1.0/(double)vmax;
    int r=255-v*(255*mm);
    return (v>vmax ? 0xff0000ff : ((0xffu << 24) | r << 16 | r << 8 | 0xff ) );
}

// Get an image from an 2D arrays of (rows,cols) with range (xmin:xmax,ymin:ymax) and
// intensity coded with colorMax as brightest.
QImage Mat2QImage(int* src, int rows, int cols,int xmin, int xmax, int ymin, int ymax,int colorMax);


#endif // COLORMAP_H
