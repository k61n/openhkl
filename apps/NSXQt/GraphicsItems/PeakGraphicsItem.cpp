#include <Eigen/Dense>

#include <QPainter>
#include <QtDebug>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

#include "IData.h"
#include "Peak3D.h"
#include "Units.h"

#include "PeakGraphicsItem.h"
#include "SXPlot.h"
#include "PeakPlot.h"

#include "Ellipsoid.h"

bool PeakGraphicsItem::_labelVisible=false;

PeakGraphicsItem::PeakGraphicsItem(SX::Crystal::Peak3D* p)
: PlottableGraphicsItem(nullptr,true,false),
  _peak(p)
{
    if (_peak)
    {
        Eigen::Vector3d c=_peak->getPeak()->getAABBCenter();
        setPos(c[0],c[1]);
    }
    _pen.setWidth(2);
    _pen.setCosmetic(true);
    _pen.setStyle(Qt::SolidLine);
    _pen.setColor(QColor(0,0,255,255));

    QString hkl;
    _label=new QGraphicsTextItem(this);
    //Ensure text is alwyas real size despite zoom
    _label->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    _label->setParentItem(this);
    setBoundingRegionGranularity(0.0);

    // A peak item is always put on foreground of the scene
    setZValue(2);
}

PeakGraphicsItem::~PeakGraphicsItem()
{
}

QRectF PeakGraphicsItem::boundingRect() const
{
//    const Eigen::Vector3d& l=_peak->getPeak()->getLower();
//    const Eigen::Vector3d& u=_peak->getPeak()->getUpper();
    const Eigen::Vector3d& l=_peak->getBackground()->getLower();
    const Eigen::Vector3d& u=_peak->getBackground()->getUpper();
    qreal w=u[0]-l[0];
    qreal h=u[1]-l[1];
    return QRectF(-w/2.0,-h/2.0,w,h);
}

void PeakGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    if (!isVisible())
        return;

    if (option->state & QStyle::State_Selected)
        _pen.setStyle(Qt::DotLine);
    else
        _pen.setStyle(Qt::SolidLine);

    painter->setRenderHint(QPainter::Antialiasing);

    if (_hovered)
        painter->setBrush(QBrush(QColor(255,255,0,120)));

    _label->setVisible(_hovered || _labelVisible);



    const Eigen::Vector3d& peak_l = _peak->getPeak()->getLower();
    const Eigen::Vector3d& peak_u = _peak->getPeak()->getUpper();
    qreal peak_w = peak_u[0]-peak_l[0];
    qreal peak_h = peak_u[1]-peak_l[1];

    const Eigen::Vector3d& bkg_l = _peak->getBackground()->getLower();
    const Eigen::Vector3d& bkg_u = _peak->getBackground()->getUpper();
    qreal bkg_w = bkg_u[0] - bkg_l[0];
    qreal bkg_h = bkg_u[1] - bkg_l[1];

    if (_peak->isSelected()) {
        _pen.setColor("green");
        painter->setPen(_pen);
        //painter->drawEllipse(0, 0, peak_w/2, peak_h/2);
        if (_peakPoints.size())
            painter->drawPolygon(&_peakPoints[0], _peakPoints.size());
        else
            painter->drawEllipse(-peak_w/2, -peak_h/2, peak_w, peak_h);

        _pen.setColor("grey");
        painter->setPen(_pen);
        //painter->drawEllipse(0, 0, bkg_w/2, bkg_h/2);
        if (_bkgPoints.size())
            painter->drawPolygon(&_bkgPoints[0], _bkgPoints.size());
        else
            painter->drawEllipse(-bkg_w/2, -bkg_h/2, bkg_w, bkg_h);
    }
    else {
        _pen.setColor("red");
        painter->setPen(_pen);
        //painter->drawEllipse(0, 0, peak_w/2, peak_h/2);
        painter->drawEllipse(-peak_w/2, -peak_h/2, peak_w, peak_h);
    }

    //painter->drawRect(-w2/2,-h2/2,w2,h2);
    _label->setPos(peak_w/2,peak_h/2);

}

void PeakGraphicsItem::setFrame(int frame)
{
    const Eigen::Vector3d& l=_peak->getPeak()->getLower();
    const Eigen::Vector3d& u=_peak->getPeak()->getUpper();
    if (frame>=l[2] && frame<=u[2]) {
        setVisible(true);
        _label->setVisible(_labelVisible);
        auto& v=_peak->getMillerIndices();
        QString hkl;
        hkl=QString("%1,%2,%3").arg(v[0]).arg(v[1]).arg(v[2]);
        _label->setPlainText(hkl);
        calculatePoints(frame);
    }
    else {
        setVisible(false);
        _label->setVisible(false);
    }
}

std::string PeakGraphicsItem::getPlotType() const
{
    return "peak";
}

SX::Crystal::Peak3D* PeakGraphicsItem::getPeak()
{
    return _peak;
}

void PeakGraphicsItem::setLabelVisible(bool flag)
{
    _labelVisible=flag;
}

void PeakGraphicsItem::calculatePoints(int frame)
{
    _peakPoints.clear();
    _bkgPoints.clear();

    const SX::Geometry::Ellipsoid<double, 3>* peak;
    const SX::Geometry::Ellipsoid<double, 3>* bkg;

    peak = dynamic_cast<const SX::Geometry::Ellipsoid<double, 3>*>(_peak->getPeak());
    bkg = dynamic_cast<const SX::Geometry::Ellipsoid<double, 3>*>(_peak->getBackground());

    // return if cannot cast to ellipsoid
    if (!peak || !bkg)
        return;

    int count = 50;

    _peakPoints.reserve(count);
    _bkgPoints.reserve(count);

    for (int i = 0; i < count; ++i) {
        double dx = std::cos((double)i / count * 2.0 * 3.141592);
        double dy = std::sin((double)i / count * 2.0 * 3.141592);

        const Eigen::Vector3d& peak_l = _peak->getPeak()->getLower();
        const Eigen::Vector3d& peak_u = _peak->getPeak()->getUpper();
        qreal peak_x = (peak_u[0] + peak_l[0])/2.0;
        qreal peak_y = (peak_u[1] + peak_l[1])/2.0;
        qreal peak_z = (peak_u[2] + peak_l[2])/2.0;

        SX::Geometry::Ellipsoid<double, 3>::vector from, dir, collide;
        double t1, t2, t;

        from << peak_x, peak_y, peak_z;
        dir << dx, dy, frame-peak_z;

        if (peak->rayIntersect(from, dir, t1, t2)) {
            t = t1 > t2? t1 : t2;
            if (t <= 0)
                break;
            collide = t*dir;
            _peakPoints.push_back(QPointF(collide(0), collide(1)));
        }

        if(bkg->rayIntersect(from, dir, t1, t2)) {
            t = t1 > t2? t1 : t2;
            if (t <= 0)
                break;
            collide = t*dir;
            _bkgPoints.push_back(QPointF(collide(0), collide(1)));
        }
    }
}

void PeakGraphicsItem::plot(SXPlot* plot)
{

    auto p=dynamic_cast<PeakPlot*>(plot);
    if (!p)
        return;

    const Eigen::VectorXd& total=_peak->getProjection();
    const Eigen::VectorXd& signal=_peak->getPeakProjection();
    const Eigen::VectorXd& bkg=_peak->getBkgProjection();

    const Eigen::VectorXd& totalSigma=_peak->getProjectionSigma();

    // Transform to QDouble
    QVector<double> qx(total.size());
    QVector<double> qtotal(total.size());
    QVector<double> qtotalE(total.size());
    QVector<double> qpeak(total.size());
    QVector<double> qbkg(total.size());

    //Copy the data
    double min=std::floor(_peak->getBackground()->getLower()[2]);
    double max=std::ceil(_peak->getBackground()->getUpper()[2]);
    if (min<0)
    	min=0;
    if (max>_peak->getData()->getNFrames()-1)
    	max=_peak->getData()->getNFrames()-1;
    for (int i=0;i<total.size();++i)
    {
        qx[i]= min + static_cast<double>(i)*(max-min)/(total.size()-1);
        qtotal[i]=total[i];
        qtotalE[i]=totalSigma[i];
        qpeak[i]=signal[i];
        qbkg[i]=bkg[i];
    }

    p->graph(0)->setDataValueError(qx, qtotal, qtotalE);
    p->graph(1)->setData(qx,qpeak);
    p->graph(2)->setData(qx,qbkg);

    // Now update text info:

    const Eigen::RowVector3d& hkl=_peak->getMillerIndices();

    QString info="(h,k,l):"+QString::number(hkl[0])+","+QString::number(hkl[1])+","+QString::number(hkl[2]);
    double gamma,nu;
    _peak->getGammaNu(gamma,nu);
    gamma/=SX::Units::deg;
    nu/=SX::Units::deg;
    info+=" "+QString((QChar) 0x03B3)+","+QString((QChar) 0x03BD)+":"+QString::number(gamma,'f',2)+","+QString::number(nu,'f',2)+"\n";
    double intensity=_peak->getScaledIntensity();
    double sI=_peak->getScaledSigma();
    info+="Intensity ("+QString((QChar) 0x03C3)+"I): "+QString::number(intensity)+" ("+QString::number(sI,'f',2)+")\n";
    double l=_peak->getLorentzFactor();
    info+="Cor. int. ("+QString((QChar) 0x03C3)+"I): "+QString::number(intensity/l,'f',2)+" ("+QString::number(sI/l,'f',2)+")\n";

    double scale=_peak->getScale();
    double monitor=_peak->getData()->getMetadata()->getKey<double>("monitor");
    info+="Monitor "+QString::number(monitor*scale)+" counts";
    QCPPlotTitle* title=dynamic_cast<QCPPlotTitle*>(p->plotLayout()->element(0,0));
    if (title)
        title->setText(info);

    p->rescaleAxes();

    p->replot();

}
