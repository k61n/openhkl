#include "ColorMap.h"

COLOUR GetColour(double v,double vmin,double vmax)
{
   COLOUR c = {1.0,1.0,1.0}; // white
   double dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v <= (vmin + 0.4 * dv)) {
      c.r = 2.5*(v-vmin) / dv;
      c.g = 0;
      c.b = 0;
   } else if (v <= (vmin + 0.8 * dv)) {
      c.r = 1;
      c.g = 1 + 2.5 * (-vmin - 0.4 * dv + v) / dv;
      c.b=  0.;
   } else {
      c.r = 1;
      c.g = 1;
      c.b = 1 + 4 * (-vmin - 0.8 * dv +v) / dv;
    }
   return c;
}

QImage Mat2QImage(int* src, int rows, int cols,double max)
{
        QImage dest(cols, rows, QImage::Format_RGB32);
        for (int y = 0; y < rows; ++y) {
            const int* srcrow = src+y;
                QRgb *destrow = reinterpret_cast<QRgb*>(dest.scanLine(y));
                for (int x = 0; x < cols; ++x) {
                        double temp=*(srcrow+x*rows);
                        COLOUR cc=GetColour(temp,0.0,max);
                        destrow[x] = qRgb(cc.r*255,cc.g*255,cc.b*255);
                }
        }
        return dest;
}
