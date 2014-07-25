#ifndef COLORMAP_H
#define COLORMAP_H
#include <QImage>

typedef struct {
    double r,g,b;
} COLOUR;


COLOUR GetColour(double v,double vmin,double vmax);

QImage Mat2QImage(int* src, int rows, int cols,double max);


#endif // COLORMAP_H
