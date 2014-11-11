#ifndef SXCUSTOMPLOT_H
#define SXCUSTOMPLOT_H

#include <qcustomplot.h>

class SXCustomPlot : public QCustomPlot
{
    Q_OBJECT
public:
    explicit SXCustomPlot(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *);
private:
    void copyViewToClipboard();

};

#endif // SXCUSTOMPLOT_H
