#include "PeakPlotter.h"
#include "ui_peakplotter.h"

PeakPlotter::PeakPlotter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PeakPlotter)
{
    ui->setupUi(this);
}

PeakPlotter::~PeakPlotter()
{
    delete ui;
}

void PeakPlotter::setPeak(const SX::Geometry::Peak3D& peak)
{
    QCustomPlot* customPlot=ui->widget;
    //
    const Eigen::VectorXd& total=peak.getProjection();
    const Eigen::VectorXd& peake=peak.getPeakProjection();
    const Eigen::VectorXd& bkge=peak.getBkgProjection();
    //
    QVector<double> qx(total.size());
    QVector<double> qtotal(total.size());
    QVector<double> qtotalE(total.size());
    QVector<double> qpeak(total.size());
    QVector<double> qbkg(total.size());

    for (int i=0;i<total.size();++i)
    {
        qx[i]=i;
        qtotal[i]=total[i];
        qtotalE[i]=sqrt(total[i]);
        qpeak[i]=peake[i];
        qbkg[i]=bkge[i];
    }
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(QColor("black")));
    customPlot->graph(0)->setErrorType(QCPGraph::etBoth);
    customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 4));
    customPlot->graph(0)->setDataValueError(qx, qtotal, qtotalE);
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(QColor("red")));
    customPlot->graph(1)->setBrush(QBrush(QColor(255,0,0,40)));
    customPlot->graph(1)->setData(qx,qpeak);
    customPlot->addGraph();
    customPlot->graph(2)->setPen(QPen(QColor("blue")));
    customPlot->graph(2)->setBrush(QBrush(QColor(0,255,0,40)));
    customPlot->graph(2)->setData(qx,qbkg);
    customPlot->rescaleAxes();
    customPlot->replot();
}
