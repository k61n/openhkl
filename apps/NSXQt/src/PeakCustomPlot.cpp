//#include "PlottableGraphicsItem.h"
//#include "PeakGraphicsItem.h"
//#include "PeakCustomPlot.h"
//#include "Peak3D.h"
//#include "IData.h"
//#include "Units.h"

//PeakCustomPlot::PeakCustomPlot(QWidget *parent) : SXCustomPlot(parent)
//{
//    plotLayout()->insertRow(0);
//    addGraph();
//    QPen pen;
//    pen.setColor(QColor("black"));
//    pen.setWidth(2.0);
//    graph(0)->setPen(pen);
//    graph(0)->setErrorType(QCPGraph::etBoth);
//    graph(0)->setLineStyle(QCPGraph::lsLine);
//    graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));

//    addGraph();
//    graph(1)->setPen(QPen(QColor("red")));
//    graph(1)->setBrush(QBrush(QColor(255,0,0,80)));

//    addGraph();
//    graph(2)->setPen(QPen(QColor("blue")));
//    graph(2)->setBrush(QBrush(QColor(0,100,255,40)));

//    xAxis->setLabel("Frame (a.u.)");
//    yAxis->setLabel("Intensity (counts)");

//    // Setup legends
//    legend->setVisible(true);
//    legend->setFont(QFont("Helvetica",9));
//    graph(0)->setName("Total");
//    graph(1)->setName("Peak");
//    graph(2)->setName("Bkg.");

//    //
//    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);

//    QCPPlotTitle* element=new QCPPlotTitle(this, "");
//    element->setFont(QFont("Arial",8,-1,true));
//    plotLayout()->addElement(0, 0, element);
//}

//void PeakCustomPlot::mousePress(QMouseEvent *event)
//{
//    Q_UNUSED(event);
//    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
//    axisRect()->setRangeDrag(xAxis->orientation());
//    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
//    axisRect()->setRangeDrag(yAxis->orientation());
//    else
//    axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
//}

//void PeakCustomPlot::mouseWheel(QWheelEvent* event)
//{
//    Q_UNUSED(event);
//    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
//      axisRect()->setRangeZoom(xAxis->orientation());
//    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
//      axisRect()->setRangeZoom(yAxis->orientation());
//    else
//      axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
//}

//void PeakCustomPlot::update(PlottableGraphicsItem* item)
//{

//    auto p=dynamic_cast<PeakGraphicsItem*>(item);
//    if (!p)
//        return;

//    SX::Crystal::Peak3D* peak=p->getPeak();

//    const Eigen::VectorXd& total=peak->getProjection();
//    const Eigen::VectorXd& signal=peak->getPeakProjection();
//    const Eigen::VectorXd& bkg=peak->getBkgProjection();

//    const Eigen::VectorXd& totalSigma=peak->getProjectionSigma();

//    // Transform to QDouble
//    QVector<double> qx(total.size());
//    QVector<double> qtotal(total.size());
//    QVector<double> qtotalE(total.size());
//    QVector<double> qpeak(total.size());
//    QVector<double> qbkg(total.size());

//    //Copy the data
//    double min=std::floor(peak->getBackground()->getLower()[2]);
//    double max=std::ceil(peak->getBackground()->getUpper()[2]);
//    for (int i=0;i<total.size();++i)
//    {
//        qx[i]= min + static_cast<double>(i)*(max-min)/(total.size()-1);
//        qtotal[i]=total[i];
//        qtotalE[i]=totalSigma[i];
//        qpeak[i]=signal[i];
//        qbkg[i]=bkg[i];
//    }

//    graph(0)->setDataValueError(qx, qtotal, qtotalE);
//    graph(1)->setData(qx,qpeak);
//    graph(2)->setData(qx,qbkg);
//    rescaleAxes();
//    replot();

//    // Now update text info:

//    const Eigen::RowVector3d& hkl=peak->getMillerIndices();

//    QString info="(h,k,l):"+QString::number(hkl[0])+","+QString::number(hkl[1])+","+QString::number(hkl[2]);
//    double gamma,nu;
//    peak->getGammaNu(gamma,nu);
//    gamma/=SX::Units::deg;
//    nu/=SX::Units::deg;
//    info+=" "+QString((QChar) 0x03B3)+","+QString((QChar) 0x03BD)+":"+QString::number(gamma,'f',2)+","+QString::number(nu,'f',2)+"\n";
//    double intensity=peak->getScaledIntensity();
//    double sI=peak->getScaledSigma();
//    info+="Intensity ("+QString((QChar) 0x03C3)+"I): "+QString::number(intensity)+" ("+QString::number(sI,'f',2)+")\n";
//    double l=peak->getLorentzFactor();
//    info+="Cor. int. ("+QString((QChar) 0x03C3)+"I): "+QString::number(intensity/l,'f',2)+" ("+QString::number(sI/l,'f',2)+")\n";

//    double scale=peak->getScale();
//    double monitor=peak->getData()->getMetadata()->getKey<double>("monitor");
//    info+="Monitor "+QString::number(monitor*scale)+" counts";
//    QCPPlotTitle* title=dynamic_cast<QCPPlotTitle*>(plotLayout()->element(0,0));
//    if (title)
//    {
//        title->setText(info);
//    }
//    replot();

//    QWidget::update();

//}
