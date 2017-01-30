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

bool PeakGraphicsItem::_labelVisible = false;
bool PeakGraphicsItem::_drawBackground = false;

PeakGraphicsItem::PeakGraphicsItem(sptrPeak3D p):
    PlottableGraphicsItem(nullptr,true,false),
    _peak(std::move(p)),
    _peakEllipse(),
    _bkgEllipse()
{
    if (_peak) {
        Eigen::Vector3d c=_peak->getPeak().getAABBCenter();
        setPos(c[0], c[1]);
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

QRectF PeakGraphicsItem::boundingRect() const
{
    const Eigen::Vector3d& l=_peak->getBackground().getLower();
    const Eigen::Vector3d& u=_peak->getBackground().getUpper();
    qreal w=u[0]-l[0];
    qreal h=u[1]-l[1];
    assert(w >= 0.0);
    assert(h >= 0.0);
    return QRectF(-w/2.0,-h/2.0,w,h);
}

void PeakGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    if (!isVisible()) {
        return;
    }
    if (option->state & QStyle::State_Selected) {
        _pen.setStyle(Qt::DotLine);
    } else {
        _pen.setStyle(Qt::SolidLine);
    }
    painter->setRenderHint(QPainter::Antialiasing);

    if (_hovered) {
        painter->setBrush(QBrush(QColor(255,255,0,120)));
    }
    _label->setVisible(_hovered || _labelVisible);

    const Eigen::Vector3d& peak_l = _peak->getPeak().getLower();
    const Eigen::Vector3d& peak_u = _peak->getPeak().getUpper();
    qreal peak_w = peak_u[0]-peak_l[0];
    qreal peak_h = peak_u[1]-peak_l[1];

    if (_peak->isSelected()) {
        _pen.setColor(_peak->isObserved() ? "green" : "yellow");
        painter->setPen(_pen);
        drawEllipse(*painter, _peakEllipse);

        if (_drawBackground) {
            _pen.setColor("grey");
            painter->setPen(_pen);
            drawEllipse(*painter, _bkgEllipse);
        }
    } else {
        _pen.setColor("red");
        painter->setPen(_pen);
        drawEllipse(*painter, _peakEllipse);
    }
    //painter->drawRect(-w2/2,-h2/2,w2,h2);
    _label->setPos(peak_w/2,peak_h/2);
}

void PeakGraphicsItem::setFrame(unsigned long frame)
{
    const Eigen::Vector3d& l=_peak->getPeak().getLower();
    const Eigen::Vector3d& u=_peak->getPeak().getUpper();

    if (frame>=l[2] && frame<=u[2]) {
        setVisible(true);
        _label->setVisible(_labelVisible);
        Eigen::RowVector3d hkl;
        bool success =_peak->getMillerIndices(hkl,true);
        QString hklString;
        hklString=QString("%1,%2,%3").arg(hkl[0]).arg(hkl[1]).arg(hkl[2]);
        _label->setPlainText(hklString);
        _peakEllipse = calculateEllipse(_peak->getPeak(), frame);
        _bkgEllipse = calculateEllipse(_peak->getBackground(), frame);
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

sptrPeak3D PeakGraphicsItem::getPeak()
{
    return _peak;
}

void PeakGraphicsItem::setLabelVisible(bool flag)
{
    _labelVisible = flag;
}

void PeakGraphicsItem::drawBackground(bool flag)
{
    _drawBackground = flag;
}

PeakGraphicsItem::Ellipse PeakGraphicsItem::calculateEllipse(const SX::Geometry::IShape<double, 3> &shape, int frame)
{
    Eigen::MatrixXd M;
    Eigen::VectorXd p;
    Ellipse ellipse;

    try {
        const SX::Geometry::Ellipsoid<double, 3>& ellipse_shape =
                dynamic_cast<const SX::Geometry::Ellipsoid<double, 3>&>(shape);
        M = ellipse_shape.getRSinv();
        p = ellipse_shape.getCenter();
    }
    catch(...){
        // bad cast, so just use information from bounding box and return early
        Eigen::Vector3d lower = shape.getLower();
        Eigen::Vector3d upper = shape.getUpper();

        ellipse.a = 0.5 * (upper[0] - lower[0]);
        ellipse.b = 0.5 * (upper[1] - lower[1]);
        ellipse.alpha = 0.0;
        ellipse.u = 0;
        ellipse.v = 0;

        return ellipse;
    }

    M = M.transpose()*M;

    // ellipsoid defined by (x-p).M.(x-p) = 1
    // rewritten as A*x^2 + B*y^2 + C*x*y + D*x*(z-z0) + E*y*(z-z0) + F = 0
    // set u = cos(alpha)*x - sin(alpha)*y
    // set v = sin(alpha)*y + cos(alpha)*x
    // and transform into standard form a*(u-u0)^2 + b*(v-v0)^2 = 1

    //const double x0 = 0.0; //p(0);
    // const double y0 = 0.0; //p(1);
    const double z0 = p(2);

    const double A = M(0,0);
    const double B = M(1,1);
    const double C = M(0,1) + M(1,0);
    const double D =  (M(0,2)+M(2,0))*(frame-z0);
    const double E =  (M(1,2)+M(2,1))*(frame-z0);
    const double F = -1.0 + M(2,2)*(frame-z0)*(frame-z0);

    const double alpha = std::atan2(0.5*C, B-A) / 2.0;

    const double c2a = std::cos(2*alpha);
    const double s2a = std::sin(2*alpha);

    const double ca = std::cos(alpha);
    const double sa = std::sin(alpha);

    const double a = std::abs(ca) > 1e-6? 0.5 * (A+B + (A-B)/c2a) : 0.5 * (A+B - C/s2a);
    const double b = A+B-a;

    const double c = (D*ca - E*sa);
    const double d = (D*sa + E*ca);

    const double u0 = -c / (2.0*a);
    const double v0 = -d / (2.0*b);

    const double e = F -a*u0*u0 - b*v0*v0;

    ellipse.a = a / -e > 0.0? 1.0 / std::sqrt(a / -e) : 0.0;
    ellipse.b = b / -e > 0.0? 1.0 / std::sqrt(b / -e) : 0.0;
    ellipse.alpha = alpha *180.0 / M_PI;
    ellipse.u = u0;
    ellipse.v = v0;

    return ellipse;
}

void PeakGraphicsItem::drawEllipse(QPainter &painter, PeakGraphicsItem::Ellipse ellipse)
{
    QPointF center(ellipse.u, ellipse.v);
    painter.rotate(-ellipse.alpha);
    painter.drawEllipse(center, ellipse.a, ellipse.b);
    painter.rotate(ellipse.alpha);
}

void PeakGraphicsItem::plot(SXPlot* plot)
{

    auto p=dynamic_cast<PeakPlot*>(plot);
    if (p == nullptr) {
        return;
    }

    const Eigen::VectorXd& total=_peak->getProjection();
    const Eigen::VectorXd& signal=_peak->getPeakProjection();
    const Eigen::VectorXd& bkg=_peak->getBkgProjection();

    const Eigen::VectorXd& totalSigma=_peak->getProjectionSigma();

    // Transform to QDouble
    QVector<double> qx(int(total.size()));
    QVector<double> qtotal(int(total.size()));
    QVector<double> qtotalE(int(total.size()));
    QVector<double> qpeak(int(total.size()));
    QVector<double> qbkg(int(total.size()));

    //Copy the data
    double min=std::floor(_peak->getBackground().getLower()[2]);
    double max=std::ceil(_peak->getBackground().getUpper()[2]);

    if (min<0) {
        min=0;
    }
    if (max>_peak->getData()->getNFrames()-1) {
        max=_peak->getData()->getNFrames()-1;
    }
    for (int i = 0; i < total.size(); ++i) {
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
    Eigen::RowVector3d hkl;
    bool success = _peak->getMillerIndices(hkl,true);

    QString info="(h,k,l):"+QString::number(hkl[0])+","+QString::number(hkl[1])+","+QString::number(hkl[2]);
    double gamma,nu;
    _peak->getGammaNu(gamma,nu);
    gamma/=SX::Units::deg;
    nu/=SX::Units::deg;
    info+=" "+QString(QChar(0x03B3))+","+QString(QChar(0x03BD))+":"+QString::number(gamma,'f',2)+","+QString::number(nu,'f',2)+"\n";
    double intensity=_peak->getScaledIntensity();
    double sI=_peak->getScaledSigma();
    info+="Intensity ("+QString(QChar(0x03C3))+"I): "+QString::number(intensity)+" ("+QString::number(sI,'f',2)+")\n";
    double l=_peak->getLorentzFactor();
    info+="Cor. int. ("+QString(QChar(0x03C3))+"I): "+QString::number(intensity/l,'f',2)+" ("+QString::number(sI/l,'f',2)+")\n";
    info += "p value (" + QString::number(_peak->pValue(), 'f', 3) + ")\n";

    double scale=_peak->getScale();
    double monitor=_peak->getData()->getMetadata()->getKey<double>("monitor");
    info+="Monitor "+QString::number(monitor*scale)+" counts";
    QCPPlotTitle* title=dynamic_cast<QCPPlotTitle*>(p->plotLayout()->element(0,0));
    if (title != nullptr) {
        title->setText(info);
    }
    p->rescaleAxes();
    p->replot();
}
