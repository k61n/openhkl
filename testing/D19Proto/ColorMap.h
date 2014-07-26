#ifndef COLORMAP_H
#define COLORMAP_H
#include <QImage>

typedef struct {
    double r,g,b;
} COLOUR;


COLOUR GetColour(double v,double vmin,double vmax);

// Get an image from an 2D arrays of (rows,cols) with range (xmin:xmax,ymin:ymax) and
// intensity coded with colorMax as brightest.
QImage Mat2QImage(int* src, int rows, int cols,int xmin, int xmax, int ymin, int ymax,double colorMax);


#endif // COLORMAP_H
