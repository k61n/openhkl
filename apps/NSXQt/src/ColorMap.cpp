#include "ColorMap.h"
#include <ctime>
#include <QtDebug>

QImage Mat2QImage(int* src, int rows, int cols,int xmin, int xmax, int ymin, int ymax,int colorMax)
{
    if (xmin<0 || xmax> cols || ymin<0 || ymax>rows)
        return QImage();


    QImage dest(xmax-xmin, ymax-ymin, QImage::Format_RGB32);

    src+=ymin*cols;
    for (int y = ymin; y < ymax; ++y)
    {
        QRgb* destrow = (QRgb*)(dest.scanLine(y-ymin));
        for (int x = xmin; x < xmax; x++)
        {
            destrow[x-xmin] = BlueWhite(*(src+x),colorMax);
        }
        src+=cols;
    }
    return dest;
}
