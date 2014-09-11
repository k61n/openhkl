#include "PeakPlotter.h"
#include "ui_peakplotter.h"
#include <IShape.h>
#include "Units.h"

PeakPlotter::PeakPlotter(QWidget *parent) :
    QDialog(parent), _current(nullptr),
    ui(new Ui::PeakPlotter)
{
    ui->setupUi(this);
    QCustomPlot* customPlot=ui->widget;
    customPlot->plotLayout()->insertRow(0);
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

    customPlot->xAxis->setLabel("Frame (a.u.)");
    customPlot->yAxis->setLabel("Intensity (counts)");

    // Setup legends
    customPlot->legend->setVisible(true);
    customPlot->legend->setFont(QFont("Helvetica",9));
    customPlot->graph(0)->setName("Total");
    customPlot->graph(1)->setName("Peak");
    customPlot->graph(2)->setName("Bkg.");

    //
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

    //
    connect(customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    QVBoxLayout* vbox = new QVBoxLayout( this );
    vbox->addWidget(ui->widget);
    ui->widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QCPPlotTitle* element=new QCPPlotTitle(customPlot, "");
    element->setFont(QFont("Arial",12,-1,true));
    customPlot->plotLayout()->addElement(0, 0, element);

}

PeakPlotter::~PeakPlotter()
{
    delete ui;
}

void PeakPlotter::copyViewToClipboard()
{
    // Create the image with the exact size of the shrunk scene
    QImage image(ui->widget->rect().size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    ui->widget->render(&painter);
    painter.end();
    QApplication::clipboard()->setImage(image,QClipboard::Clipboard);
}

void PeakPlotter::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Copy))
    {
        copyViewToClipboard();
    }
}

void PeakPlotter::setPeak(SX::Geometry::Peak3D *peak)
{
    _current = peak;
    update();
}

void PeakPlotter::update()
{
    QCustomPlot* customPlot=ui->widget;
    // Get the data
    const Eigen::VectorXd& total=_current->getProjection();
    const Eigen::VectorXd& peak=_current->getPeakProjection();
    const Eigen::VectorXd& bkg=_current->getBkgProjection();

    const Eigen::VectorXd& totalSigma=_current->getProjectionSigma();
    const Eigen::VectorXd& peakSigma=_current->getPeakProjectionSigma();
    const Eigen::VectorXd& bkgSigma=_current->getBkgProjectionSigma();

    // Transform to QDouble
    QVector<double> qx(total.size());
    QVector<double> qtotal(total.size());
    QVector<double> qtotalE(total.size());
    QVector<double> qpeak(total.size());
    QVector<double> qbkg(total.size());

    //Copy the data
    double min=std::floor(_current->getBackground()->getLower()[2]);
    double max=std::ceil(_current->getBackground()->getUpper()[2]);
    for (int i=0;i<total.size();++i)
    {
        qx[i]= min + static_cast<double>(i)*(max-min)/(total.size()-1);
        qtotal[i]=total[i];
        qtotalE[i]=totalSigma[i];
        qpeak[i]=peak[i];
        qbkg[i]=bkg[i];
    }

    customPlot->graph(0)->setDataValueError(qx, qtotal, qtotalE);
    customPlot->graph(1)->setData(qx,qpeak);
    customPlot->graph(2)->setData(qx,qbkg);
    customPlot->rescaleAxes();
    customPlot->replot();

    // Now update text info:

    const Eigen::RowVector3d& hkl=_current->getMillerIndices();

    QString info="(h,k,l):"+QString::number(hkl[0])+","+QString::number(hkl[1])+","+QString::number(hkl[2])+"\n";
    double gamma=_current->getGamma()/SX::Units::deg;
    double nu=_current->getNu()/SX::Units::deg;
    info+=QString((QChar) 0x03B3)+","+QString((QChar) 0x03BD)+":"+QString::number(gamma,'f',2)+","+QString::number(nu,'f',2)+"\n";
    double intensity=_current->getScaledIntensity();
    double sI=_current->getScaledSigma();
    info+="Intensity ("+QString((QChar) 0x03C3)+"I): "+QString::number(intensity)+" ("+QString::number(sI,'f',2)+")\n";
    double l=_current->getLorentzFactor();
    info+="Cor. int. ("+QString((QChar) 0x03C3)+"I): "+QString::number(intensity/l,'f',2)+" ("+QString::number(sI/l,'f',2)+")\n";


    QCPPlotTitle* title=dynamic_cast<QCPPlotTitle*>(customPlot->plotLayout()->element(0,0));
    if (title)
    {
        title->setText(info);
    }
    customPlot->replot();

    QDialog::update();
}

void PeakPlotter::mousePress()
{
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

    QCustomPlot* customPlot=ui->widget;

  if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot->axisRect()->setRangeDrag(customPlot->xAxis->orientation());
  else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot->axisRect()->setRangeDrag(customPlot->yAxis->orientation());
  else
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void PeakPlotter::mouseWheel()
{
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

    QCustomPlot* customPlot=ui->widget;

  if (customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot->axisRect()->setRangeZoom(customPlot->xAxis->orientation());
  else if (customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
    customPlot->axisRect()->setRangeZoom(customPlot->yAxis->orientation());
  else
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

