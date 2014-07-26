#include "Plotter1d.h"
#include "ui_plotter1d.h"
#include "qwt_plot_magnifier.h"
#include "qwt_plot_zoomer.h"
#include <iostream>
#include "qwt_plot_histogram.h"
Plotter1D::Plotter1D(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plotter1D)
{
    ui->setupUi(this);
    QwtPlot* plot=ui->qwtPlot;
    plot->setAxisTitle(QwtPlot::xBottom,"Omega");
    plot->setAxisTitle(QwtPlot::yLeft,"Counts");
    plot->setCanvasBackground(QColor(Qt::white));
    QwtPlotZoomer* zoomer= new QwtPlotZoomer(ui->qwtPlot->canvas());
    zoomer->setRubberBandPen(QPen(QColor(Qt::red)));
    zoomer->setTrackerPen(QColor(Qt::black));

}

Plotter1D::~Plotter1D()
{
    delete ui;
}

void Plotter1D::addCurve(const std::vector<double> &x, const std::vector<double> &y)
{
    _x.resize(x.size());
    _y.resize(y.size());
    std::copy(x.begin(),x.end(),_x.begin());
    std::copy(y.begin(),y.end(),_y.begin());
    auto itx=std::max_element(_x.begin(),_x.end());
    auto ity=std::max_element(_y.begin(),_y.end());
    ui->qwtPlot->axisAutoScale(QwtPlot::xBottom);
    ui->qwtPlot->axisAutoScale(QwtPlot::yLeft);
    QwtPlotCurve *curve1 = new QwtPlotCurve("Curve 1");
    curve1->setPen(QPen(QColor(Qt::red)));
    curve1->setRawSamples(&(_x[0]),&(_y[0]),_x.size());
    curve1->setRenderHint(QwtPlotItem::RenderAntialiased,true);
    curve1->attach(ui->qwtPlot);
    ui->qwtPlot->replot();

}
