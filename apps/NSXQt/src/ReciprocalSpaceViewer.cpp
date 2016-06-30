#include "include/ReciprocalSpaceViewer.h"
#include "ui_ReciprocalSpaceViewer.h"

#include <limits>
#include <set>
#include <stdexcept>

#include "Axis.h"
#include "Detector.h"
#include "Diffractometer.h"
#include "Experiment.h"
#include "Gonio.h"
#include "IData.h"
#include "MonoDetector.h"
#include "Sample.h"
#include "Source.h"
#include "UnitCell.h"
#include "Logger.h"

ReciprocalSpaceViewer::ReciprocalSpaceViewer(SX::Instrument::Experiment* experiment,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReciprocalSpaceViewer),
    _experiment(experiment),
    _data()
{
    ui->setupUi(this);

    int nUnitCells(_experiment->getDiffractometer()->getSample()->getNCrystals());

    if (!nUnitCells)
        throw std::runtime_error("No unit cells defined for the sample");

    ui->unitCell->setMaximum(nUnitCells);
}

ReciprocalSpaceViewer::~ReciprocalSpaceViewer()
{
    delete ui;
}

void ReciprocalSpaceViewer::setData(const std::vector<SX::Data::IData*>& data)
{
    _data.clear();
    _data.reserve(data.size());

    for (auto d : data)
    {
        std::string basename(d->getBasename());
        if (_experiment->hasData(basename))
            _data.push_back(_experiment->getData(basename));
    }
}

void ReciprocalSpaceViewer::on_view_clicked()
{
    double dq(ui->dq->value());
    if (dq < 0.0)
        throw std::runtime_error("Negative q resolution");

    double halfwidth(ui->width->value()/2.0);
    if (halfwidth < 0.0)
        throw std::runtime_error("Negative plane width");

    int h1(ui->h1Dir->value());
    int k1(ui->k1Dir->value());
    int l1(ui->l1Dir->value());
    if (h1==0 && k1==0 && l1==0)
        throw std::runtime_error("The direction 1 for defining the slice plane is the null vector");

    int h2(ui->h2Dir->value());
    int k2(ui->k2Dir->value());
    int l2(ui->l2Dir->value());
    if (h2==0 && k2==0 && l2==0)
        throw std::runtime_error("The direction 2 for defining the slice plane is the null vector");

    Eigen::Vector3d v1(static_cast<double>(h1),static_cast<double>(k1),static_cast<double>(l1));
    Eigen::Vector3d v2(static_cast<double>(h2),static_cast<double>(k2),static_cast<double>(l2));

//    std::shared_ptr<SX::Crystal::UnitCell> uc(_experiment->getDiffractometer()->getSample()->getUnitCell(ui->unitCell->value()));

//    Eigen::Matrix3d UB=uc->getReciprocalReferenceM().transpose();

    Eigen::Matrix3d UB=Eigen::Matrix3d::Identity();

    Eigen::Vector3d from(static_cast<double>(ui->hOrig->value()),static_cast<double>(ui->kOrig->value()),static_cast<double>(ui->lOrig->value()));
    from = UB*from;

    v1 = UB*v1;
    v2 = UB*v2;

    Eigen::Vector3d v3(v1.cross(v2));
    v3.normalize();

    if (v3.norm() < 1.0e-6)
        throw std::runtime_error("The two vectors used for defining the slice plane are colinear");

    // The distance from the point origin to the plane (P,v1,v2)
    double distOrigToPlane=v3.dot(from);

    SX::Instrument::Detector* detector(_experiment->getDiffractometer()->getDetector());
    int nDetRows = detector->getNRows();
    int nDetCols = detector->getNCols();

    SX::Instrument::Sample* sample(_experiment->getDiffractometer()->getSample());

    double lambda(_experiment->getDiffractometer()->getSource()->getWavelength());
    double invlambda(1.0/lambda);

    double invdq(1.0/dq);

    std::vector<std::pair<Eigen::Vector3d,double>> ptsWithinPlanes;
    ptsWithinPlanes.reserve(nDetRows*nDetCols);

    std::vector<Eigen::Vector3d> qrest;
    qrest.reserve(nDetRows*nDetCols);

    SX::Instrument::MonoDetector* mdetector=dynamic_cast<SX::Instrument::MonoDetector*>(detector);
    double pixelS=mdetector->getPixelWidth()*mdetector->getPixelHeigth();

    for (int j=0;j<nDetCols;++j)
    {
        double px=static_cast<double>(j)+0.5;
        for (int i=0;i<nDetRows;++i)
        {
            double py=static_cast<double>(i)+0.5;
            qrest.push_back(detector->getPos(px,py).normalized()*invlambda);
        }
    }

    auto axes = detector->getGonio()->getAxes();

    for (auto d : _data)
    {

        for (int f=0;f<d->getNFrames();++f)
        {
            auto detectorStates(d->getDetectorState(f).getValues());
            Eigen::Matrix3d invM(sample->getGonio()->getInverseHomMatrix(d->getSampleState(f).getValues()).rotation());

            Eigen::Transform<double,3,Eigen::Affine> hmatrix=Eigen::Transform<double,3,Eigen::Affine>::Identity();
            std::vector<SX::Instrument::Axis*>::const_reverse_iterator it;
            std::vector<double>::const_reverse_iterator itv=detectorStates.rbegin();

            for (it=axes.rbegin();it!=axes.rend();++it)
            {
                if ((*it)->isPhysical())
                {
                    hmatrix=(*it)->getHomMatrix(*itv)*hmatrix;
                    itv++;
                }
                else
                    hmatrix=(*it)->getHomMatrix(0.0)*hmatrix;
            }

            const Eigen::MatrixXi& frame = d->readFrame(f);

            for (int j=0;j<nDetCols;++j)
            {
                for (int i=0;i<nDetRows;++i)
                {
                    Eigen::Vector3d q=hmatrix*qrest[j*nDetRows+i].homogeneous();

                    q[1]-=invlambda;

                    q = invM*q;

                    double distQToPlane=(std::fabs(v3.dot(q)+distOrigToPlane));
                    if (distQToPlane <= halfwidth)
                    {
                        double density=static_cast<double>(frame(i,j))/pixelS;
                        ptsWithinPlanes.push_back(std::pair<Eigen::Vector3d,double>(q,density));
                    }
                }
            }
        }
    }

    double minx(std::numeric_limits<double>::infinity());
    double miny(std::numeric_limits<double>::infinity());

    double maxx(-std::numeric_limits<double>::infinity());
    double maxy(-std::numeric_limits<double>::infinity());

    for (const auto& point: ptsWithinPlanes)
    {
        auto pt=point.first;
        if (pt[0] < minx)
            minx = pt[0];
        if (pt[0] > maxx)
            maxx = pt[0];

        if (pt[1] < miny)
            miny = pt[1];
        if (pt[1] > maxy)
            maxy = pt[1];
    }

    int nqx=static_cast<int>((maxx-minx)/dq)+2;
    int nqy=static_cast<int>((maxy-miny)/dq)+2;

    Eigen::MatrixXd slice=Eigen::MatrixXd::Zero(nqx,nqy);

    for (const auto& point : ptsWithinPlanes)
    {
        Eigen::Vector3d pt=point.first;
        double intensity(point.second);

        double ptv1=pt.dot(v1)-minx;
        double ptv2=pt.dot(v2)-miny;

        double binx=std::floor(ptv1*invdq);
        double biny=std::floor(ptv2*invdq);

        double s((ptv1-binx)/dq);
        double t((ptv2-biny)/dq);

        double I0=(1.0-s)*(1.0-t)*intensity;
        double I1=s*(1.0-t)*intensity;
        double I2=(1.0-s)*t*intensity;
        double I3=s*t*intensity;

        int ipx=static_cast<int>(binx);
        int ipy=static_cast<int>(biny);

        slice(ipx,ipy) += I0;
        slice(ipx+1,ipy) += I1;
        slice(ipx,ipy+1) += I2;
        slice(ipx+1,ipy+1) += I3;
    }

    qDebug()<<slice.rows()<<" "<<slice.cols()<<minx<<maxx<<miny<<maxy<<nqx<<nqy;

    // configure axis rect
    ui->plot->clearGraphs();
    ui->plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    ui->plot->axisRect()->setupFullAxesBox(true);
    ui->plot->xAxis->setLabel("x");
    ui->plot->yAxis->setLabel("y");

    QCPColorMap *colorMap = new QCPColorMap(ui->plot->xAxis, ui->plot->yAxis);
    colorMap->clearData();
    ui->plot->addPlottable(colorMap);

    // Set a color map  of nqx * nqy data points
    colorMap->data()->setSize(nqx, nqy);

    // Make it span the slice min and max values along x and y axis
    colorMap->data()->setRange(QCPRange(minx, maxx), QCPRange(miny, maxy));

    // Assign the data
    for (int i=0; i<nqx; ++i)
    {
        for (int j=0; j<nqy; ++j)
            colorMap->data()->setCell(i, j, slice(i,j));
    }

    // Add a color scale:
    QCPColorScale *colorScale = new QCPColorScale(ui->plot);
    // Add it to the right of the main axis rect
    ui->plot->plotLayout()->addElement(0, 1, colorScale);
    // Scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorScale->setType(QCPAxis::atRight);
    // Associate the color map with the color scale
    colorMap->setColorScale(colorScale);
    colorScale->axis()->setLabel("Intensity");

    // Rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // Make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->plot);
    ui->plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // Rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->plot->rescaleAxes();
}
