#include "PeakPlotter.h"
#include "ui_peakplotter.h"
#include <IShape.h>

PeakPlotter::PeakPlotter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PeakPlotter)
{
    ui->setupUi(this);
    QCustomPlot* customPlot=ui->widget;
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(customPlot, "Peak:"));
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(QColor("black")));
    customPlot->graph(0)->setErrorType(QCPGraph::etBoth);
    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 4));

    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(QColor("red")));
    customPlot->graph(1)->setBrush(QBrush(QColor(255,0,0,80)));

    customPlot->addGraph();
    customPlot->graph(2)->setPen(QPen(QColor("blue")));
    customPlot->graph(2)->setBrush(QBrush(QColor(0,0,255,40)));

    // Setup legends
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica",9));
    customPlot->graph(0)->setName("Total");
    customPlot->graph(1)->setName("Peak");
    customPlot->graph(2)->setName("Bkg.");
}

PeakPlotter::~PeakPlotter()
{
    delete ui;
}

void PeakPlotter::setPeak(const SX::Geometry::Peak3D& peak)
{
    QCustomPlot* customPlot=ui->widget;
    // Get the data
    const Eigen::VectorXd& total=peak.getProjection();
    const Eigen::VectorXd& peake=peak.getPeakProjection();
    const Eigen::VectorXd& bkge=peak.getBkgProjection();
    // Transform to QDouble
    QVector<double> qx(total.size());
    QVector<double> qtotal(total.size());
    QVector<double> qtotalE(total.size());
    QVector<double> qpeak(total.size());
    QVector<double> qbkg(total.size());

    //Copy the data
    for (int i=0;i<total.size();++i)
    {
        qx[i]=i;
        qtotal[i]=total[i];
        qtotalE[i]=sqrt(total[i]);
        qpeak[i]=peake[i];
        qbkg[i]=bkge[i];
    }

    customPlot->graph(0)->setDataValueError(qx, qtotal, qtotalE);
    customPlot->graph(1)->setData(qx,qpeak);
    customPlot->graph(2)->setData(qx,qbkg);
    customPlot->rescaleAxes();
    customPlot->replot();

    // Now update text info:

    const Eigen::RowVector3d& hkl=peak.getMillerIndices();
    const SX::Geometry::IShape<double,3>* shape=peak.getPeak();

    QString info="(h,k,l):"+QString::number(hkl[0])+","+QString::number(hkl[1])+","+QString::number(hkl[2])+"\n";
    double intensity=peak.peakTotalCounts();
    double sI=sqrt(intensity);
    info+="Intensity ("+QString((QChar) 0x03C3)+"I): "+QString::number(intensity)+" ("+QString::number(sI,'f',2)+")\n";
    double l=peak.getLorentzFactor();
    info+="Lor. Cor. int. ("+QString((QChar) 0x03C3)+"I): "+QString::number(intensity/l,'f',2)+" ("+QString::number(sI/l,'f',2)+")\n";
    ui->textBrowser->setText(info);
}
