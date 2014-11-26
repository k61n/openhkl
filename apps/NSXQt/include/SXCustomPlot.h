#ifndef SXCUSTOMPLOT_H
#define SXCUSTOMPLOT_H

#include <string>

#include <qcustomplot.h>

class PlottableGraphicsItem;

class SXCustomPlot : public QCustomPlot
{
    Q_OBJECT
public:

    static SXCustomPlot* create(QWidget* parent);

    explicit SXCustomPlot(QWidget *parent = 0);
    void keyPressEvent(QKeyEvent *);
    virtual ~SXCustomPlot();

    // Getters and setters

    virtual std::string getType() const;

public slots:
    void mousePress(QMouseEvent *event);
    void mouseWheel(QWheelEvent *event);
    virtual void update(PlottableGraphicsItem* item);

private:
    void copyViewToClipboard();

};

#endif // SXCUSTOMPLOT_H
