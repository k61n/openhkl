#ifndef SIMPLECUSTOMPLOT_H
#define SIMPLECUSTOMPLOT_H

#include <string>

#include "SXCustomPlot.h"

class QWidget;

class SimplePlot : public SXCustomPlot
{
    Q_OBJECT
public:

    static SXCustomPlot* create(QWidget* parent);

    explicit SimplePlot(QWidget *parent = 0);

    // Getters and setters
    std::string getType() const;

};

#endif // SIMPLECUSTOMPLOT_H
