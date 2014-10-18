#include "Plotter1D.h"
#include "ui_plotter1d.h"
#include <QClipboard>

Plotter1D::Plotter1D(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Plotter1D)
{
    ui->setupUi(this);

}

Plotter1D::~Plotter1D()
{
    delete ui;
}

void Plotter1D::addCurve(const QVector<double> &x, const QVector<double> &y, const QVector<double> &e, QColor color)
{
    QCustomPlot* customPlot=ui->plot;
    customPlot->addGraph();
    customPlot->graph()->setPen(QPen(color));
    customPlot->graph()->setErrorPen(QPen(color));
    customPlot->graph()->setErrorType(QCPGraph::etBoth);
    customPlot->graph()->setLineStyle(QCPGraph::lsNone);
    customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 4));
    customPlot->graph()->setDataValueError(x, y, e);
    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iMultiSelect | QCP::iSelectPlottables );
    customPlot->rescaleAxes();
    customPlot->replot();
}

void Plotter1D::modifyCurve(int number, const QVector<double> &x, const QVector<double> &y, const QVector<double> &e)
{
    QCustomPlot* customPlot=ui->plot;
    QCPGraph* graph=customPlot->graph(number);
    if (!graph)
        return;
    graph->setDataValueError(x, y, e);
    customPlot->rescaleAxes();
    customPlot->replot();
}

void Plotter1D::setXlabel(const std::string& label)
{
    QCustomPlot* customPlot=ui->plot;
    customPlot->xAxis->setLabel(QString::fromStdString(label));
}


void Plotter1D::setYlabel(const std::string& label)
{
    QCustomPlot* customPlot=ui->plot;
    customPlot->yAxis->setLabel(QString::fromStdString(label));
}

void Plotter1D::removeCurve(int idx)
{
    ui->plot->removeGraph(idx);
    ui->plot->replot();
}

int Plotter1D::nGraphs() const
{
    return ui->plot->graphCount();
}

void Plotter1D::clear()
{
    ui->plot->clearGraphs();
    ui->plot->replot();
}
