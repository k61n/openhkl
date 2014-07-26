#include "Plotter1D.h"
#include "ui_plotter1d.h"
#include <QClipboard>

Plotter1D::Plotter1D(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plotter1D)
{
    ui->setupUi(this);
    QCustomPlot* customPlot=ui->plot;
    customPlot->addGraph();
    QVector<double> x(100);
    QVector<double> y(100);
    QVector<double> z(100);
    for (int i=0;i<100;++i)
    {
        x[i]=i;
        y[i]=i*i;
        z[i]=i;
    }
    customPlot->graph(0)->setErrorPen(QPen(QColor("red")));
    customPlot->graph(0)->setErrorType(QCPGraph::etBoth);
    customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    customPlot->graph(0)->setDataValueError(x, y, z);
    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(0,100);
    customPlot->yAxis->setRange(0, 10000);
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
                                      QCP::iMultiSelect | QCP::iSelectPlottables );
    customPlot->replot();
    QPixmap pm=customPlot->toPixmap();
    QClipboard * clipboard = QApplication::clipboard();
    clipboard->setPixmap(pm);
}

Plotter1D::~Plotter1D()
{
    delete ui;
}
